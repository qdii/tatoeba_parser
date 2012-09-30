#ifndef LINKSET_H
#define LINKSET_H

#include <vector>
#include "sentence.h"

NAMESPACE_START

/**@struct linkset
 * @brief A container that represents the links between the sentences */
struct linkset
{
public:
    linkset( size_t _nbSentences, size_t _nbLinks );

    static const size_t NB_MAX_LINKS = 5000000;

public:
    void addLink( sentence::id _a, sentence::id _b );
    bool areLinked( sentence::id _a, sentence::id _b ) const;
    const sentence::id * getLinksOf( sentence::id _a ) const;

    typedef std::vector<sentence::id> linksArray;

private:
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
    std::vector< sentence::id * > m_pointers;

private:
    linkset( const linkset & );
    linkset & operator=( const linkset & );
};

// -------------------------------------------------------------------------- //

inline
void linkset::addLink( sentence::id _a, sentence::id _b ) __restrict
{
    static size_t indexInLinkArray = 0;
    static sentence::id lastIdValue = sentence::INVALID_ID;

    if( lastIdValue != _a )
    {
        lastIdValue = _a;
        m_pointers[_a] = &m_links[++indexInLinkArray];
    }

    m_links[indexInLinkArray++] = _b;
}

// -------------------------------------------------------------------------- //

inline
bool linkset::areLinked( sentence::id _a, sentence::id _b ) const __restrict
{
    sentence::id * iter = m_pointers[_a];

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
    return m_pointers[_a];
}

NAMESPACE_END
#endif //LINKSET_H
