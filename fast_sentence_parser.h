#ifndef FAST_SENTENCE_PARSER_H
#define FAST_SENTENCE_PARSER_H

#include "parser.h"
#include "file_mapper.h"

NAMESPACE_START

struct fastSentenceParser : public parser
{
    fastSentenceParser( char * _begin, char * _end );
    int start() throw () override;
};

NAMESPACE_END
#endif // FAST_SENTENCE_PARSER_H
