#include "prec.h"
#include "fast_tag_parser.h"

NAMESPACE_START

int fastTagParser::start() throw()
{
    qlog::trace << "parsing tags.\n";
    dataset & __restrict data = * getDataset();
    register char * __restrict begin = getMapBegin();
    register char * const __restrict end = getMapEnd();

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
