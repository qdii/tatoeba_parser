#ifndef FAST_LINK_PARSER_H
#define FAST_LINK_PARSER_H

#include "parser.h"

NAMESPACE_START

struct fastLinkParser : parser
{
    fastLinkParser( const fileMapper & _linkMap ): parser(_linkMap) { }
    
    int start() throw() override;
};

NAMESPACE_END
#endif // FAST_LINK_PARSER_H
