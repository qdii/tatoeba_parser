#ifndef FAST_TAG_PARSER_H
#define FAST_TAG_PARSER_H

#include "parser.h"

NAMESPACE_START

struct fastTagParser : public parser
{
    fastTagParser( char * _begin, char * _end )
        :parser( _begin, _end )
    {
    }
    int start() throw() override;
};

NAMESPACE_END

#endif //FAST_TAG_PARSER_H
