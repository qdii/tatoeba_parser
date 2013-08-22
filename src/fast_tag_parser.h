#ifndef LIBTATOPARSER_FAST_TAG_PARSER_H
#define LIBTATOPARSER_FAST_TAG_PARSER_H

#include "tatoparser/tagset.h"

#pragma GCC visibility push(hidden)

NAMESPACE_START

template<typename iterator>
struct fastTagParser
{
    fastTagParser( iterator _begin, iterator _end )
        :m_begin( _begin )
        ,m_end( _end )
        ,m_abort( false )
    {
    }

    int start( tagset & _tagset ) TATO_NO_THROW;
    size_t countTags();

    /**@brief Cancels a parsing operation */
    void abort() { m_abort = true; }

private:
    iterator m_begin, m_end;
    bool m_abort;
};

// -------------------------------------------------------------------------- //

template<typename iterator>
size_t fastTagParser<iterator>::countTags()
{
    return std::count( m_begin, m_end, '\n' );
}

// -------------------------------------------------------------------------- //


template<typename iterator>
int fastTagParser<iterator>::start( tagset & TATO_RESTRICT _tagset ) TATO_NO_THROW
{
    register iterator cursor = m_begin;
    register iterator const end = m_end;

    sentence::id sentenceId = sentence::INVALID_ID;
    tagset temporaryTagContainer;

    for ( char * tagName = nullptr; cursor != end && !m_abort; )
    {
        sentenceId = static_cast<sentence::id>( *cursor++ - '0' );

        // parsing the sentence id
        while( *cursor != '\t' )
        {
            assert( *cursor >= '0' && *cursor <= '9' );
            sentenceId = sentenceId * 10 + static_cast<sentence::id>( *cursor++ - '0' );
        }

        // skip '\t'
        ++cursor;
        assert( cursor != end );

        // record character string for tag name
        tagName = cursor;

        // skip to the end of the string
        while ( ++cursor != end && *cursor != '\n' );

        assert( cursor < end );

        // replace the new line character with a NULL character to have a C string
        *cursor++ = '\0';

        // add an entry
        try
        {
            temporaryTagContainer.tagSentence( sentenceId, tagName );
        }
        catch( const std::bad_alloc & )
        {
            llog::error << "Out of memory.\n";
            break;
        }
    }

    if( cursor == end )
        _tagset = std::move( temporaryTagContainer );

    return 0;
}

NAMESPACE_END

#pragma GCC visibility pop

#endif //LIBTATOPARSER_FAST_TAG_PARSER_H
