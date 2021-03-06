#ifndef LIBTATOPARSER_FAST_LINK_PARSER_H
#define LIBTATOPARSER_FAST_LINK_PARSER_H

#include <algorithm>
#include "tatoparser/namespace.h"
#include "tatoparser/linkset.h"

#pragma GCC visibility push(hidden)

NAMESPACE_START

/**@struct fastLinkParser
 * @tparam iterator An input iterator to read into a buffer */
template<typename iterator>
struct fastLinkParser
{
    /**@brief The POD type that contains the number of lines returned by countLines */
    typedef typename std::iterator_traits<iterator>::difference_type nb_of_lines;

    /**@brief Constructs a fastLinkParser
     * @param[in] _begin An iterator to the beginning of the buffer to parse
     * @param[in] _end An iterator to the end of the buffer to parse */
    fastLinkParser( iterator _begin, iterator _end )
        : m_begin( _begin )
        , m_end( _end )
        , m_abort( false )
    {
    }

    /**@brief Parses the file
     * @return The number of links parsed
     * @param[in] allLinks_ A container that will be filled with the links */
    nb_of_lines start( linkset & allLinks_ ) TATO_NO_THROW;

    /**@brief Counts the lines in the file */
    nb_of_lines countLines() const;

    /**@brief Cancels a parsing operation */
    void abort() { m_abort = true; }

private:
    iterator m_begin, m_end;
    volatile bool m_abort;
};

// -------------------------------------------------------------------------- //

template<typename iterator> inline
typename fastLinkParser<iterator>::nb_of_lines
fastLinkParser<iterator>::countLines() const
{
    return std::count( m_begin, m_end, '\n' );
}

// -------------------------------------------------------------------------- //

template<typename iterator>
typename fastLinkParser<iterator>::nb_of_lines
fastLinkParser<iterator>::start( linkset & TATO_RESTRICT allLinks_ ) TATO_NO_THROW
{
    nb_of_lines nbLinks = 0;
    register iterator ptr = m_begin;
    iterator ptrEnd = m_end;
    linkset temporaryLinkContainer;

    if( ptr == nullptr || ptr == ptrEnd )
        return 0;

    sentence::id firstId = 0;
    sentence::id id = 0;

    for (register char c; ptr != ptrEnd && !m_abort;)
    {
        c = *ptr++;

        if( ( c != '\t' ) && ( c != '\n' ) )
        {
            assert( c >= '0' && c <= '9');

            // if the line is 125\t234\n
            // then we "build" id the following way:
            // c = '1'   ->    id = 10 * 0 + 1  = 1
            // c = '2'   ->    id = 10 * 1 + 2  = 12
            // c = '5'   ->    id = 10 * 12 + 5 = 125
            id = 10 * id + static_cast<sentence::id>(c - '0');
        }
        else
        {
            if( c == '\t' )
            {
                firstId = id;
            }
            else if( c == '\n' )
            {
                try
                {
                    temporaryLinkContainer.addLink( firstId, id );
                    ++nbLinks;
                }
                catch ( std::bad_alloc & )
                {
                    llog::error << "Not enough memory.\n";
                    break;
                }
            }

            id = 0;
        }
    }

    // if no error occurred, we swap containers
    if ( ptrEnd == ptr )
    {
        allLinks_ = std::move( temporaryLinkContainer );
    }

    llog::info << "parsed " << nbLinks << " links.\n";

    return nbLinks;
}

NAMESPACE_END

#pragma GCC visibility pop

#endif // FAST_LINK_PARSER_H
