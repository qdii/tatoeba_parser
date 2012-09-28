#ifndef DATASET_H
#define DATASET_H

#include <array>
#include <vector>
#include "sentence.h"
#include "linkset.h"
NAMESPACE_START

/**@struct dataset
 * @brief A structure that stores all the sentences and the links */
struct dataset
{
    static const size_t NB_MAX_SENTENCES = 2000000;
    static const size_t NB_MAX_LINKS     = 100;
    static const size_t HIGHER_ID        = 2500370;
    typedef std::array<sentence, NB_MAX_SENTENCES> containerType;

public:
    typedef typename containerType::iterator iterator;
    typedef typename containerType::const_iterator const_iterator;

    typedef const sentence::id * linksArray;
    typedef std::array< std::array< sentence::id, NB_MAX_LINKS >, HIGHER_ID > linksContainer;
    typedef std::array<sentence *, HIGHER_ID> fastAccessArray;

public:
    dataset()
        :m_allSentences( new containerType )
        ,m_allLinks( nullptr )
        ,m_fastAccess( nullptr )
    {
    }

    ~dataset()
    {
        delete m_allSentences;
        delete m_allLinks;
        delete m_fastAccess;
    }

    void allocateMemoryForLinks(size_t _nbSentences, size_t _nbLinks)
    {
        m_allLinks = new linkset(_nbSentences, _nbLinks);
    }

public:
    iterator begin() { return m_allSentences->begin(); }
    const_iterator begin() const { return m_allSentences->begin(); }

    iterator end() { return m_allSentences->end(); }
    const_iterator end() const { return m_allSentences->end(); }
    dataset::linksArray getLinksOf( sentence::id _sentence ) const;
    void addSentence( sentence::id _a, const char * _lang, const char * _data );

public:
    sentence * operator[]( sentence::id );

    /**@brief Should be run before any sentence is retrieved using operator[]
     */
    void prepare();

public:
    void addLink( sentence::id _a, sentence::id _b ) { m_allLinks->addLink(_a,_b); }
    bool areLinked( sentence::id _a, sentence::id _b ) const { return m_allLinks->areLinked(_a, _b); }

private:
    dataset( const dataset & ) = delete;
    dataset & operator=( const dataset & ) = delete;

private:
    containerType   * m_allSentences;
    linkset         * m_allLinks;
    fastAccessArray * m_fastAccess;
};

// -------------------------------------------------------------------------- //

inline
sentence * dataset::operator[]( sentence::id _id )
{
    sentence * ret = nullptr;

    if( m_fastAccess )
        ret = ( *m_fastAccess )[_id];
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
dataset::linksArray dataset::getLinksOf( sentence::id _sentence ) const
{
    return m_allLinks->getLinksOf(_sentence);
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
        ( *fastArray )[it->getId()] = &*it;
        ++it;
    }

    m_fastAccess = fastArray;
}

// -------------------------------------------------------------------------- //

inline
void dataset::addSentence(sentence::id _id, const char * _lang, const char * _data)
{
    static size_t sentenceNumber = 0;
    new (&(*m_allSentences)[sentenceNumber++]) sentence(_id, _lang, _data);

}

NAMESPACE_END
#endif // DATASET_H
