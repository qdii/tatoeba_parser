#ifndef TATOPARSER_INTERFACE_LIB_H
#define TATOPARSER_INTERFACE_LIB_H

NAMESPACE_START

// -------------------------------------------------------------------------- //
struct dataset;
struct linkset;
struct tagset;
// -------------------------------------------------------------------------- //
typedef uint32_t ParserFlag;

static const ParserFlag VERBOSE    = 1<<0;
static const ParserFlag DEBUG      = 1<<1;
static const ParserFlag NO_TAGS    = 1<<2;  // do not parse tags.csv
static const ParserFlag NO_LINKS   = 1<<3;  // do not parse links.csv


// -------------------------------------------------------------------------- //
int init( ParserFlag _flags );
int parse( dataset & allSentences_,
           linkset & allLinks_,
           tagset  & allTags_,
           const std::string & sentencePath,
           const std::string & linksPath,
           const std::string & tagPath );

int terminate();

NAMESPACE_END

#endif // TATOPARSER_INTERFACE_LIB_H
