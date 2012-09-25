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
    static const size_t NB_MAX_LINKS = 300;
    static const size_t HIGHER_ID = 3500370;
    typedef std::array<sentence, NB_MAX_SENTENCES> containerType;
    typedef typename containerType::iterator iterator;
    typedef typename containerType::const_iterator const_iterator;

public:
    typedef std::array< sentence::id, NB_MAX_LINKS > linksVector;
    typedef std::array< std::array< sentence::id, NB_MAX_LINKS >, HIGHER_ID > linksContainer;
    typedef std::array<sentence *, HIGHER_ID> fastAccessArray;

public:
    dataset()
        :m_allSentences( new containerType )
        ,m_allLinks( new linksContainer )
        ,m_fastAccess(nullptr)
    {
        linksContainer::iterator endLinksContainer = m_allLinks->end();
        for (linksContainer::iterator sentenceIterator = m_allLinks->begin(); sentenceIterator != endLinksContainer; ++sentenceIterator)
        {
            *(sentenceIterator->begin()) = sentence::INVALID_ID;
        }
    }

    ~dataset()
    {
        delete m_allSentences;
        delete m_allLinks;
        delete m_fastAccess;
    }

public:
    iterator begin() { return m_allSentences->begin(); }
    const_iterator begin() const { return m_allSentences->begin(); }

    iterator end() { return m_allSentences->end(); }
    const_iterator end() const { return m_allSentences->end(); }

    void addLink( sentence::id _a, sentence::id _b );
    bool areLinked( sentence::id _a, sentence::id _b ) const;
    const linksVector & getLinksOf( sentence::id _sentence ) const;

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
    linksContainer * m_allLinks;
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
void dataset::addLink( sentence::id _a, sentence::id _b )
{
    linksVector & __restrict vectorLinks = ( *m_allLinks )[_a];
    size_t i = 0;
    while (vectorLinks[i++] != sentence::INVALID_ID)
        ;
    vectorLinks[i--] = sentence::INVALID_ID;
    vectorLinks[i] = _b;
}

// -------------------------------------------------------------------------- //

inline
bool dataset::areLinked( sentence::id _a, sentence::id _b ) const __restrict
{
    for (auto it = (*m_allLinks)[_a].begin() ; it != (*m_allLinks)[_a].end(); ++it)
    {
        if ((*it != _b) && (*it != sentence::INVALID_ID))
            continue;
        else if (*it == _b)
            return true;
        else
            return false;

    }
    assert(0);
    return false;
}

// -------------------------------------------------------------------------- //

inline
const dataset::linksVector & dataset::getLinksOf( sentence::id _sentence ) const
{
    return (*m_allLinks)[_sentence];
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
