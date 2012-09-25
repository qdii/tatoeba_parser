#ifndef FAST_LINK_PARSER_H
#define FAST_LINK_PARSER_H

#include "parser.h"

NAMESPACE_START

struct fastLinkParser : parser
{
    fastLinkParser( char * _begin, char * _end ): parser( _begin, _end ) { }

    int start() throw() override;
};

NAMESPACE_END
#endif // FAST_LINK_PARSER_H
