#ifndef DATASET_H
#define DATASET_H

#include <vector>
#include "namespace.h"
#include "sentence.h"
#include "datainfo.h"

NAMESPACE_START

/**@struct dataset
 * @brief A structure that stores all the sentences */
struct dataset
{
    /**@brief Constructs a dataset */
    dataset();

    typedef std::vector<sentence> containerType;

    typedef typename containerType::iterator iterator;
    typedef typename containerType::const_iterator const_iterator;

    // fastAccessArray stores index of sentences
    typedef std::vector<size_t> fastAccessArray;

public:
    void allocate( const datainfo & _info );

public:
    iterator begin() { return m_allSentences.begin(); }
    const_iterator begin() const { return m_allSentences.begin(); }

    iterator end() { return m_allSentences.end(); }
    const_iterator end() const { return m_allSentences.end(); }
    void addSentence( sentence::id _a, const char * _lang, const char * _data );

public:
    sentence * operator[]( sentence::id );
    const sentence * operator[]( sentence::id ) const;

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
    assert( !m_fastAccess.empty() ); // if m_fastAccess is empty, it means that prepare() command has not been run before.
    if (m_fastAccess[_id] == static_cast<size_t>(-1))
        return nullptr;

    return & ( m_allSentences[ m_fastAccess[_id] ] );
}

// -------------------------------------------------------------------------- //

inline
const sentence * dataset::operator[]( sentence::id _id ) const
{
    assert( !m_fastAccess.empty() );
    if (m_fastAccess[_id] == static_cast<size_t>(-1))
        return nullptr;

    return & ( m_allSentences[ m_fastAccess[_id] ] );
}

// -------------------------------------------------------------------------- //

inline
void dataset::addSentence( sentence::id _id, const char * _lang, const char * _data )
{
    m_allSentences.push_back( sentence( _id, _lang, _data ) );
}

NAMESPACE_END
#endif // DATASET_H
