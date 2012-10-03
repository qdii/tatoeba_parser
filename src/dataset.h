#ifndef DATASET_H
#define DATASET_H

#include <array>
#include <vector>
#include "sentence.h"
#include "datainfo.h"

NAMESPACE_START

/**@struct dataset
 * @brief A structure that stores all the sentences */
struct dataset
{
    dataset()
        :m_allSentences()
        ,m_fastAccess()
    {
    }

    typedef std::vector<sentence> containerType;

    typedef typename containerType::iterator iterator;
    typedef typename containerType::const_iterator const_iterator;

    // fastAccessArray stores index of sentences
    typedef std::vector<size_t> fastAccessArray;

public:
    void allocate( const datainfo & _info )
    {
        assert( _info.m_nbSentences > 0 );
        m_allSentences.reserve( _info.m_nbSentences );

        qlog::info << "Allocated " << ( m_allSentences.capacity() * sizeof( sentence ) )/ ( 1024*1024 )<< " MB for sentences.\n";
    }

public:
    iterator begin() { return m_allSentences.begin(); }
    const_iterator begin() const { return m_allSentences.begin(); }

    iterator end() { return m_allSentences.end(); }
    const_iterator end() const { return m_allSentences.end(); }
    void addSentence( sentence::id _a, const char * _lang, const char * _data );

public:
    sentence * operator[]( sentence::id );

    /**@brief Should be run before any sentence is retrieved using operator[]
     */
    void prepare( const datainfo & _info );

private:
    dataset( const dataset & ) = delete;
    dataset & operator=( const dataset & ) = delete;

private:
    containerType   m_allSentences;
    fastAccessArray m_fastAccess;
};

// -------------------------------------------------------------------------- //

inline
sentence * dataset::operator[]( sentence::id _id )
{
    assert( _id < static_cast<sentence::id>( m_fastAccess.size() ) ); // if m_fastAccess is empty, it means that prepare() command has not been run before.
    assert( m_fastAccess[_id] < m_allSentences.size() );
    return & ( m_allSentences[ m_fastAccess[_id] ] );
}

// -------------------------------------------------------------------------- //

inline
void dataset::prepare( const datainfo & _info ) __restrict
{
    m_fastAccess.reserve( _info.m_highestId + 1 );
    const size_t nbSentences = m_allSentences.size();

    for( size_t index = 0; index < nbSentences; ++index )
    {
        const sentence & __restrict curSentence = m_allSentences[ index ];
        assert( curSentence.getId() != sentence::INVALID_ID );
        assert( curSentence.getId() < static_cast<sentence::id>(m_fastAccess.capacity()) );
        m_fastAccess[curSentence.getId()] = index;
    }
}

// -------------------------------------------------------------------------- //

inline
void dataset::addSentence( sentence::id _id, const char * _lang, const char * _data )
{
    m_allSentences.push_back( sentence( _id, _lang, _data ) );
}

NAMESPACE_END
#endif // DATASET_H
