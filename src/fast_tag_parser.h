#ifndef FAST_TAG_PARSER_H
#define FAST_TAG_PARSER_H

#include "parser.h"

NAMESPACE_START

template<typename iterator>
struct fastTagParser : public parser<iterator>
{
    fastTagParser( char * _begin, char * _end )
        :parser<iterator>( _begin, _end )
    {
    }
    int start() throw() override;
};

// -------------------------------------------------------------------------- //


template<typename iterator>
int fastTagParser<iterator>::start() throw()
{
    qlog::trace << "parsing tags.\n";
    dataset & __restrict data = * parser<iterator>::getDataset();
    register iterator __restrict begin = parser<iterator>::getMapBegin();
    register iterator const __restrict end = parser<iterator>::getMapEnd();

    sentence::id sentenceId = sentence::INVALID_ID;
    char * tagName = nullptr;

    while( begin != end )
    {
        sentenceId = *begin++ - '0';

        // parsing the sentence id
        while( *begin != '\t' )
        {
            assert( *begin >= '0' && *begin <= '9' );
            sentenceId = sentenceId * 10 + ( *begin++ - '0' );
        }

        // skip '\t'
        ++begin;
        assert( begin != end );

        // record character string for tag name
        tagName = begin;

        // skip to the end of the string
        while( *++begin != '\n' )
            ;

        assert( begin < end );

        // replace the new line character with a NULL character to have a C string
        *begin++ = '\0';

        // add an entry
        data.addTag( sentenceId, tagName );
    }

    qlog::trace << "done parsing tags.\n";

    return 0;
}

NAMESPACE_END

#endif //FAST_TAG_PARSER_H
