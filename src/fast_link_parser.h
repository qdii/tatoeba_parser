#ifndef FAST_LINK_PARSER_H
#define FAST_LINK_PARSER_H

#include <algorithm>

NAMESPACE_START

struct linkset;

/**@struct fastLinkParser
 * @tparam iterator An input iterator to read into a buffer */
template<typename iterator>
struct fastLinkParser
{
    /**@brief Constructs a fastLinkParser
     * @param[in] _begin An iterator to the beginning of the buffer to parse
     * @param[in] _end An iterator to the end of the buffer to parse */
    fastLinkParser( iterator _begin, iterator _end )
        : m_begin( _begin )
        , m_end( _end )
    {
    }

    /**@brief Parses the file
     * @return The number of links parsed
     * @param[in] allLinks_ A container that will be filled with the links */
    size_t start( linkset & allLinks_ ) TATO_NO_THROW;

    /**@brief Counts the lines in the file */
    size_t countLines() const;

private:
    iterator m_begin, m_end;
};

// -------------------------------------------------------------------------- //

template<typename iterator> inline
size_t fastLinkParser<iterator>::countLines() const
{
    return std::count( m_begin, m_end, '\n' );
}

// -------------------------------------------------------------------------- //

template<typename iterator>
size_t fastLinkParser<iterator>::start( linkset & TATO_RESTRICT allLinks_ ) TATO_NO_THROW
{
    size_t nbLinks = 0;
    register iterator TATO_RESTRICT ptr = m_begin;
    iterator ptrEnd = m_end;

    if( ptr == nullptr || ptr == ptrEnd )
        return 0;

    sentence::id firstId = 0;
    sentence::id id = 0;
    register char c;

    while( ptr != ptrEnd )
    {
        c = *ptr++;

        if( ( c != '\t' ) && ( c != '\n' ) )
        {
            assert( c >= '0' && c <= '9');

            // if the line is 125\t234\n
            // then we "build" id in the following manner:
            // c = '1'   ->    id = 10 * 0 + 1  = 1
            // c = '2'   ->    id = 10 * 1 + 2  = 12
            // c = '5'   ->    id = 10 * 12 + 5 = 125
            id = 10 * id + c - '0';
        }
        else
        {
            if( c == '\t' )
            {
                firstId = id;
            }
            else if( c == '\n' )
            {
                allLinks_.addLink( firstId, id );
                ++nbLinks;
            }

            id = 0;
        }
    }

    llog::info << "parsed all links.\n";

    return nbLinks;
}

NAMESPACE_END
#endif // FAST_LINK_PARSER_H
