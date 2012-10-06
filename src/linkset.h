#ifndef LINKSET_H
#define LINKSET_H

#include <vector>
#include <stdexcept>
#include "sentence.h"

NAMESPACE_START

struct datainfo;

/**@struct linkset
 * @brief A container that represents the links between the sentences */
struct linkset
{
public:
    /**@brief Constructs a linkset object
     * @throw std::bad_alloc */
    linkset() ;

    /**@brief allocates memory to store the links in
      *@param[in] _datainfo Informations about the number of links that will be stored
      *@throw std::bad_alloc   */
    void allocate(const datainfo & _datainfo );

    /**@brief register a link between two sentences
     * @param[in] _a The first sentence
     * @param[in] _b The second sentence */
    void addLink( sentence::id _a, sentence::id _b );

    /**@brief checks if two sentences are linked
     * @param[in] _a The first sentence
     * @param[in] _b The second sentence
     * @return True if the two sentences are linked */
    bool areLinked( sentence::id _a, sentence::id _b ) const;

    /**@brief retrieves all the ids of the sentences linked to a sentence
     * @param[in] _a A sentence
     * @return A pointer to a null terminated vector of ids */
    const sentence::id * getLinksOf( sentence::id _a ) const;

private:
    typedef std::vector<sentence::id> linksArray;
    /// A first idea was to create a matrix of bits, each line representing a
    /// sentence A and each column representing a sentence B. If the intersection
    /// of the line and the column is a 1, then they are linked, else they are
    /// not. But that ends up taking at least NB_SENTENCES*NB_SENTENCES bits,
    /// which is some millions of gigabytes size.

    /// Another idea is to have a first array of pointers, which is indexed by
    /// the sentence id. The pointer drops into an array of sentence ids, which
    /// is terminated by INVALID_SENTENCE_ID. Each sentence id represents a link
    /// of the sentence.
    linksArray                    m_links;
    std::vector< size_t >         m_pointers;

private:
    linkset( const linkset & );
    linkset & operator=( const linkset & );
};

// -------------------------------------------------------------------------- //

inline
void linkset::addLink( sentence::id _a, sentence::id _b ) TATO_RESTRICT
{
    static sentence::id lastIdValue = sentence::INVALID_ID;

    if( lastIdValue != _a )
    {
        lastIdValue = _a;
        assert( _a < static_cast<sentence::id>( m_pointers.size() ) );
        m_links.push_back( 0 );
        m_pointers[_a] = m_links.size();
    }

    m_links.push_back( _b );
}

// -------------------------------------------------------------------------- //

inline
bool linkset::areLinked( sentence::id _a, sentence::id _b ) const TATO_RESTRICT
{
    /** @todo : recode with iterators */
    const sentence::id * iter = &m_links[m_pointers[_a]];

    if( iter == nullptr )
        return false;

    while( *iter != sentence::INVALID_ID )
    {
        if( *iter++ == _b )
            return true;
    }

    return false;
}

// -------------------------------------------------------------------------- //

inline
const sentence::id * linkset::getLinksOf( sentence::id _a ) const
{
    assert( _a < static_cast<sentence::id>( m_pointers.size() ) );
    return &m_links[m_pointers[_a]];
}

NAMESPACE_END
#endif //LINKSET_H
