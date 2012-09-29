#ifndef DATASET_H
#define DATASET_H

#include <array>
#include <vector>
#include "sentence.h"
#include "linkset.h"
#include "tagset.h"
NAMESPACE_START

/**@struct dataset
 * @brief A structure that stores all the sentences and the links */
struct dataset
{
    static const size_t NB_MAX_SENTENCES = 2000000;
    static const size_t HIGHER_ID        = 2500370;
    typedef std::vector<sentence> containerType;

    typedef const sentence::id * linksArray;
    typedef typename containerType::iterator iterator;
    typedef typename containerType::const_iterator const_iterator;

    typedef std::array<sentence *, HIGHER_ID> fastAccessArray;

public:
    dataset()
        :m_allSentences( nullptr )
        ,m_allLinks( nullptr )
        ,m_fastAccess( nullptr )
    {
    }

    ~dataset()
    {
        delete m_allSentences;
        delete m_allLinks;
        delete m_fastAccess;
        delete m_allTags;
    }

    void allocateMemoryForLinks( size_t _nbLinks )
    {
        m_allLinks = new linkset( m_nbSentences, _nbLinks );
    }

    void allocateMemoryForSentences( size_t _nbSentences )
    {
        m_nbSentences = _nbSentences;
        m_allSentences = new containerType;
        m_allSentences->reserve( _nbSentences );

        qlog::info << "Allocated " << ( m_allSentences->capacity() * sizeof( sentence ) )/ ( 1024*1024 )<< " MB for sentences.\n";
    }

    void allocateMemoryForTags( size_t _nbTags )
    {
        m_allTags = new tagset( m_nbSentences, _nbTags );
    }

public:
    iterator begin() { return m_allSentences->begin(); }
    const_iterator begin() const { return m_allSentences->begin(); }

    iterator end() { return m_allSentences->end(); }
    const_iterator end() const { return m_allSentences->end(); }
    dataset::linksArray getLinksOf( sentence::id _sentence ) const;
    void addSentence( sentence::id _a, const char * _lang, const char * _data );
    void addTag( sentence::id _sentenceId, const char * _tag ) { m_allTags->tagSentence( _sentenceId, _tag ); }

    tagset::tagId getTagId( const std::string _name ) { return m_allTags->getTagId( _name ); }
    bool hasTag( sentence::id _sentenceId, tagset::tagId _tag ) const { return m_allTags->isSentenceTagged( _sentenceId, _tag ); }

public:
    sentence * operator[]( sentence::id );

    /**@brief Should be run before any sentence is retrieved using operator[]
     */
    void prepare();

public:
    void addLink( sentence::id _a, sentence::id _b ) { m_allLinks->addLink( _a,_b ); }
    bool areLinked( sentence::id _a, sentence::id _b ) const { return m_allLinks->areLinked( _a, _b ); }

private:
    dataset( const dataset & ) = delete;
    dataset & operator=( const dataset & ) = delete;

private:
    containerType  *  m_allSentences;
    linkset     *     m_allLinks;
    tagset      *     m_allTags;
    fastAccessArray * m_fastAccess;
    size_t            m_nbSentences;
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
    return m_allLinks->getLinksOf( _sentence );
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
void dataset::addSentence( sentence::id _id, const char * _lang, const char * _data )
{
    m_allSentences->push_back( sentence( _id, _lang, _data ) );
}

NAMESPACE_END
#endif // DATASET_H
