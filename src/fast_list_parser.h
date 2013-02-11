#ifndef LIBTATOPARSER_FAST_LIST_PARSER_H
#define LIBTATOPARSER_FAST_LIST_PARSER_H

#include <algorithm>
#include "tatoparser/namespace.h"

#pragma GCC visibility push(hidden)

NAMESPACE_START

struct listset;

/**@struct fastListParser
 * @tparam iterator An input iterator to read into a buffer */
template<typename iterator>
struct fastListParser
{
    /**@brief Constructs a fastListParser
     * @param[in] _begin An iterator to the beginning of the buffer to parse
     * @param[in] _end An iterator to the end of the buffer to parse */
    fastListParser( iterator _begin, iterator _end )
        : m_begin( _begin )
        , m_end( _end )
    {
    }

    /**@brief Parses the file
     * @return The number of lists parsed
     * @param[in] allLists A container that will be filled with the links */
    size_t start( listset & allLinks_ ) TATO_NO_THROW;

    /**@brief Counts the lines in the file */
    size_t countLines() const;

private:
    iterator m_begin, m_end;
};

// -------------------------------------------------------------------------- //

template<typename iterator> inline
size_t fastListParser<iterator>::countLines() const
{
    return std::count( m_begin, m_end, '\n' );
}

// -------------------------------------------------------------------------- //

template<typename iterator>
size_t fastListParser<iterator>::start( listset & TATO_RESTRICT allLists_ ) TATO_NO_THROW
{
    llog::info << "parsing lists.csv\n";

    size_t lineCount = 0;
    bool parsingId = true;
    listset temporaryListContainer;

    sentence::id current_id = 0;
    iterator current_pos, name_begin;
    char c;

    for( current_pos = m_begin; current_pos != m_end; ++current_pos )
    {
        c = *current_pos;
        if( ( c != '\t' ) && ( c != '\n' ) )
        {
            if( parsingId )
                current_id = 10 * current_id + static_cast<sentence::id>( c - '0' );
        }
        else if( c == '\t' )
        {
            assert( parsingId );
            parsingId = false;
            name_begin = current_pos+1;
        }
        else if( c == '\n' )
        {
            assert( !parsingId );
            *current_pos = '\0';
            try
            {
                temporaryListContainer.addSentenceToList( current_id, std::string( name_begin, current_pos ) );
                ++lineCount;
            }
            catch( std::bad_alloc & )
            {
                llog::error << "Out of memory\n";
                break;
            }
            current_id = 0;
            parsingId = true;
        }
    }

    // if no error occurred, we switch containers
    if ( current_pos == m_end )
        allLists_ = std::move( temporaryListContainer );

    llog::info << "parsed " << lineCount << " lines.\n";
    return lineCount;
}

NAMESPACE_END

#pragma GCC visibility pop

#endif // FAST_LINK_PARSER_H
