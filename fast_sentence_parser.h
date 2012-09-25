#ifndef FAST_SENTENCE_PARSER_H
#define FAST_SENTENCE_PARSER_H
NAMESPACE_START

#include "parser.h"
#include "file_mapper.h"

struct fastSentenceParser : public parser
{
    fastSentenceParser( char * _begin, char * _end );
    int start() throw () override;
};

NAMESPACE_END
#endif // FAST_SENTENCE_PARSER_H
