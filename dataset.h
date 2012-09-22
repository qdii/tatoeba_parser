#ifndef DATASET_H
#define DATASET_H

#include <array>
#include <vector>
#include "sentence.h"

NAMESPACE_START

struct dataset
{
private:
    static const size_t NB_MAX_SENTENCES = 800000;
    static const size_t HIGHER_ID = 3500370;
    typedef std::array<sentence, NB_MAX_SENTENCES> containerType;
    typedef typename containerType::iterator iterator;
    typedef typename containerType::const_iterator const_iterator;

    typedef std::vector< std::vector< sentence::id > > linksContainer;
    typedef std::array<sentence *, HIGHER_ID> fastAccessArray;

public:
    dataset() 
        :m_allSentences( new containerType )
        ,m_allLinks()
        ,m_fastAccess(nullptr)
    {
        m_allLinks.resize( HIGHER_ID );
    }

    ~dataset()
    {
        delete m_allSentences;
        delete m_fastAccess;
    }

public:
    iterator begin() { return m_allSentences->begin(); }
    const_iterator begin() const { return m_allSentences->begin(); }

    iterator end() { return m_allSentences->end(); }
    const_iterator end() const { return m_allSentences->end(); }

    void addLink( sentence::id _a, sentence::id _b );
    bool areLinked( sentence::id _a, sentence::id _b ) const;
    const std::vector<sentence::id> & getLinksOf( sentence::id _sentence ) const;

public:
    sentence * operator[]( sentence::id );

    /**@brief Should be run before any sentence is retrieved using operator[]
     */
    void prepare();

private:
    dataset( const dataset & ) = delete;
    dataset & operator=( const dataset & ) = delete;

private:
    containerType  * m_allSentences;
    linksContainer   m_allLinks;
    fastAccessArray * m_fastAccess;
};

// -------------------------------------------------------------------------- //

inline
sentence * dataset::operator[]( sentence::id _id )
{
    sentence * ret = nullptr;

    if( m_fastAccess )
        ret = (*m_fastAccess)[_id];
    else
    {
        qlog::warning << "dataset::prepare() was not called before operator[].\n";
        iterator it =
            std::find_if(
                begin(), end(),
                [_id]( sentence& _candidate )
                {
                    return _candidate.getId() == _id;
                }
            );

        if( it != end() )
            ret = &*it;
    }

    return ret;
}

// -------------------------------------------------------------------------- //

inline
void dataset::addLink( sentence::id _a, sentence::id _b ) __restrict
{
    m_allLinks[_a].push_back( _b );
}

// -------------------------------------------------------------------------- //

inline
bool dataset::areLinked( sentence::id _a, sentence::id _b ) const
{
    return std::find( m_allLinks[_a].begin(), m_allLinks[_a].end(), _b ) != m_allLinks[_a].end();
}

// -------------------------------------------------------------------------- //

inline
const std::vector<sentence::id> & dataset::getLinksOf( sentence::id _sentence ) const
{
    return m_allLinks[_sentence];
}

// -------------------------------------------------------------------------- //

inline
void dataset::prepare()
{
    fastAccessArray * __restrict fastArray = new fastAccessArray;
    auto it = m_allSentences->begin();
    auto endSentence = m_allSentences->end();
    
    while( it != endSentence && it->getId() != sentence::INVALID_ID )
    {
        (*fastArray)[it->getId()] = &*it;
        ++it;
    }
    
    m_fastAccess = fastArray;
}

NAMESPACE_END
#endif // DATASET_H
