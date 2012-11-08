#ifndef TATOPARSER_INTERFACE_LIB_H
#define TATOPARSER_INTERFACE_LIB_H

#include <cstdint>
#include <string>
#include "namespace.h"

NAMESPACE_START

// -------------------------------------------------------------------------- //
struct dataset;
struct linkset;
struct tagset;
// -------------------------------------------------------------------------- //
typedef uint32_t ParserFlag;

static const ParserFlag NONE       = 0;
static const ParserFlag VERBOSE    = 1<<0;
static const ParserFlag DEBUG      = 1<<1;
static const ParserFlag NO_TAGS    = 1<<2;  // do not parse tags.csv
static const ParserFlag NO_LINKS   = 1<<3;  // do not parse links.csv
static const ParserFlag DETAILED   = 1<<4;  // sentencePath is a detailed sentence file


// -------------------------------------------------------------------------- //
int init( ParserFlag _flags );
int parse( dataset & allSentences_,
           linkset & allLinks_,
           tagset  & allTags_,
           const std::string & _sentencePath,
           const std::string & _linksPath,
           const std::string & _tagPath );

int terminate();

NAMESPACE_END

#endif // TATOPARSER_INTERFACE_LIB_H
