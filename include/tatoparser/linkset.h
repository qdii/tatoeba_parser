#ifndef LINKSET_H
#define LINKSET_H

#include <vector>
#include <stdexcept>
#include <utility>
#include <algorithm>
#include "namespace.h"
#include "sentence.h"

NAMESPACE_START

struct datainfo;
struct dataset;

/**@struct linkset
 * @brief A container that represents the links between the sentences */
struct linkset
{
public:
    typedef sentence::id      *     iterator;
    typedef const sentence::id   *  const_iterator;

    /**@brief Constructs a linkset object
     * @throw std::bad_alloc */
    linkset() ;

    /**@brief allocates memory to store the links in
      *@param[in] _datainfo Informations about the number of links that will be stored
      *@throw std::bad_alloc   */
    void allocate( const datainfo & _datainfo );

    /**@brief register a link between two sentences
     * @param[in] _a The first sentence
     * @param[in] _b The second sentence
     * @throw std::bad_alloc   */
    void addLink( sentence::id _a, sentence::id _b );

    /**@brief checks if two sentences are linked
     * @param[in] _a The first sentence
     * @param[in] _b The second sentence
     * @return True if the two sentences are linked */
    bool areLinked( sentence::id _a, sentence::id _b ) const;

    /**@brief retrieves all the ids of the sentences linked to a sentence
     * @param[in] _a A sentence
     * @return A pair of iterator (begin,end), traversing a sequence of sentence::ids */
    std::pair<const_iterator, const_iterator> getLinksOf( sentence::id _a ) const;

    /**@brief Find the highest id in the list of links
     * @return a sentence id */
    sentence::id getHighestSentenceId() const;

private:
    typedef std::vector<sentence::id> linksArray;
    /// A first idea was to create a matrix of bits, each line representing a
    /// sentence A and each column representing a sentence B. If the intersection
    /// of the line and the column is a 1, then they are linked, else they are
    /// not. But that ends up taking at least NB_SENTENCES*NB_SENTENCES bits,
    /// which is some millions of gigabytes size.

    /// m_links contains a list of sentence::ids. It is actually the concatenation
    /// of different lists of sentence::ids. If m_links is [ 3 4 5 2 1 4 ], then
    /// it could be the concatenation of [ 3 4 ], [ 5 ], and [ 2 1 4 ], because
    /// sentence id 1 can be translated to 3 and 4, and sentence id 15 can be
    /// translated into 2 1 and 4. m_offsets gives you the delimiters.
    linksArray                                  m_links;

    /// m_offset is a vector of delimiters. It is indexed by sentence ids.
    /// For instance, if m_offsets is: [ (0,2) (2,3) (0,0), ... ], it means that
    /// sentence id 1 has two delimiters, 0 and 2
    /// sentence id 2 has two delimiters, 2 and 3
    ///
    /// Those delimiters are offsets in m_links. If m_links is [ 3 4 5 2 1 4 ],
    /// sentence id 1’s offsets are 0 and 2, meaning that the sub list [ 3 4 5 ],
    /// which begins at the index 0 and ends at the offset 2, are the translations
    /// of sentence 1.
    std::vector< std::pair<size_t, size_t> >    m_offsets;

private:
    linkset( const linkset & );
    linkset & operator=( const linkset & );
};

// -------------------------------------------------------------------------- //

/**@brief Retrieve the first translation of a sentence in a given language.
 * @param[in] _dataset Container for all the sentences.
 * @param[in] _linkset Container for all the links.
 * @param[in] _sentence The id of the sentence which translation are retrieved.
 * @param[in] _lang The language to translate the sentence in
 * @return The id of the first translation, or sentence::INVALID_ID  */
sentence::id getFirstSentenceTranslation(
    const dataset & _dataset,
    const linkset & _linkset,
    sentence::id _sentence,
    const std::string & _lang
);

// -------------------------------------------------------------------------- //

inline
void linkset::addLink( sentence::id _a, sentence::id _b ) TATO_RESTRICT
{
    static sentence::id lastIdValue = sentence::INVALID_ID;

    if( lastIdValue != _a )
    {
        lastIdValue = _a;
        while ( _a >= static_cast<sentence::id>( m_offsets.size() ) )
        {
            m_offsets.resize( 2 * m_offsets.size() );
        }
        m_offsets[_a].first = m_links.size();
    }

    m_links.push_back( _b );
    m_offsets[_a].second = m_links.size();
}

// -------------------------------------------------------------------------- //

inline
bool linkset::areLinked( sentence::id _a, sentence::id _b ) const TATO_RESTRICT
{
    assert( _a != sentence::INVALID_ID );
    assert( _b != sentence::INVALID_ID );
    const std::pair< const_iterator, const_iterator > & translationsOfA =
        getLinksOf( _a );

    return std::find( translationsOfA.first, translationsOfA.second, _b )
           != translationsOfA.second;
}

// -------------------------------------------------------------------------- //

inline
std::pair<linkset::const_iterator, linkset::const_iterator>
linkset::getLinksOf( sentence::id _a ) const
{
    assert( _a < m_offsets.size() );
    const std::pair<size_t, size_t> & sentenceOffsets = m_offsets[_a];
    assert( sentenceOffsets.first < m_links.size() );
    assert( sentenceOffsets.second <= m_links.size() );
    return std::pair<linkset::const_iterator, linkset::const_iterator>(
               m_links.data() + sentenceOffsets.first, m_links.data() + sentenceOffsets.second
           );
}

NAMESPACE_END
#endif //LINKSET_H
