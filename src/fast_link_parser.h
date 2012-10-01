#ifndef FAST_LINK_PARSER_H
#define FAST_LINK_PARSER_H

#include "parser.h"

NAMESPACE_START

template<typename iterator>
struct fastLinkParser : parser<iterator>
{
    fastLinkParser( iterator _begin, iterator _end )
        : parser<iterator>( _begin, _end )
    {
    }

    int start() throw() TATO_OVERRIDE;
};

// -------------------------------------------------------------------------- //

template<typename iterator>
int fastLinkParser<iterator>::start() throw()
{
    register iterator __restrict ptr = parser<iterator>::getMapBegin();
    iterator ptrEnd = parser<iterator>::getMapEnd();

    dataset & __restrict data = * parser<iterator>::getDataset();

    if( ptr == nullptr || ptr == ptrEnd )
        return 0;

    sentence::id firstId = 0;
    sentence::id id = 0;
    int nbLinks = 0;
    register char c;

    while( ptr != ptrEnd )
    {
        c = *ptr++;

        if( ( c != '\t' ) && ( c != '\n' ) )
        {
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
                data.addLink( firstId, id );
                ++nbLinks;
            }

            id = 0;
        }
    }

    qlog::info << "parsed " << nbLinks << " links\n";

    return nbLinks;
}

NAMESPACE_END
#endif // FAST_LINK_PARSER_H
