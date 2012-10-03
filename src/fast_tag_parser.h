#ifndef FAST_TAG_PARSER_H
#define FAST_TAG_PARSER_H

NAMESPACE_START

template<typename iterator>
struct fastTagParser
{
    fastTagParser( iterator _begin, iterator _end )
        :m_begin( _begin )
        ,m_end( _end )
    {
    }

    int start( tagset & _tagset ) TATO_NO_THROW;
    size_t countTags();

private:
    iterator m_begin, m_end;
};

// -------------------------------------------------------------------------- //

template<typename iterator>
size_t fastTagParser<iterator>::countTags()
{
    return std::count( m_begin, m_end, '\n' );
}

// -------------------------------------------------------------------------- //


template<typename iterator>
int fastTagParser<iterator>::start( tagset & __restrict _tagset ) TATO_NO_THROW
{
    register iterator __restrict cursor = m_begin;
    register iterator const __restrict end = m_end;

    sentence::id sentenceId = sentence::INVALID_ID;
    char * tagName = nullptr;

    while( cursor != end )
    {
        sentenceId = *cursor++ - '0';

        // parsing the sentence id
        while( *cursor != '\t' )
        {
            assert( *cursor >= '0' && *cursor <= '9' );
            sentenceId = sentenceId * 10 + ( *cursor++ - '0' );
        }

        // skip '\t'
        ++cursor;
        assert( cursor != end );

        // record character string for tag name
        tagName = cursor;

        // skip to the end of the string
        while( *++cursor != '\n' )
            ;

        assert( cursor < end );

        // replace the new line character with a NULL character to have a C string
        *cursor++ = '\0';

        // add an entry
        _tagset.tagSentence( sentenceId, tagName );
    }

    qlog::trace << "done parsing tags.\n";

    return 0;
}

NAMESPACE_END

#endif //FAST_TAG_PARSER_H
