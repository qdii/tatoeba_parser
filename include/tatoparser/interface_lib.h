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
struct listset;
// -------------------------------------------------------------------------- //
typedef uint32_t ParserFlag;

////////////////////////
//   PARSING FLAGS    //
////////////////////////
static const ParserFlag NONE       = 0;

// logs info messages to std::err (instead of only errors)
static const ParserFlag VERBOSE    = 1<<0;

// adds extra sanity checks
static const ParserFlag DEBUG      = 1<<1;

// partially parses the corpus (omitting tags.csv file)
static const ParserFlag NO_TAGS    = 1<<2;

// partially parses the corpus  (omitting links.csv)
static const ParserFlag NO_LINKS   = 1<<3;

// partially parses the corpus (omitting lists.csv)
static const ParserFlag NO_LISTS   = 1<<4;

// expects a sentences_detailed.csv file and parses the author of the sentences along with
// the rest of the data
static const ParserFlag DETAILED   = 1<<5;


// -------------------------------------------------------------------------- //

/**@brief Initializes the parser
 * @return EXIT_SUCCESS on success */
int init( ParserFlag _flags );

/**@brief Parses the database
 * @param[out] allSentences_ A structure which will be filled with all the sentences
 * @param[out] allLinks_ A structure which will be filled with information about links.
 * @param[out] allTags_ A structure which will be filled with information about tags.
 * @param[in] _sentencePath A path to sentences.csv
 * @param[in] _linksPath A path to links.csv
 * @param[in] _tagPath A path to tags.csv
 * @param[in] _listPath A path to lists.csv
 * @return EXIT_SUCCESS on success, EXIT_FAILURE otherwise */
int parse_( dataset & allSentences_,
           linkset & allLinks_,
           tagset  & allTags_,
           listset & allLists_,
           const char * _sentencePath,
           const char * _linksPath,
           const char * _tagPath,
           const char * _listPath );

/**@brief Parses the database
 * @param[out] allSentences_ A structure which will be filled with all the sentences
 * @param[out] allLinks_ A structure which will be filled with information about links.
 * @param[out] allTags_ A structure which will be filled with information about tags.
 * @param[in] _sentencePath A path to sentences.csv
 * @param[in] _linksPath A path to links.csv
 * @param[in] _tagPath A path to tags.csv
 * @param[in] _listPath A path to lists.csv
 * @return EXIT_SUCCESS on success, EXIT_FAILURE otherwise */
int parse( dataset & allSentences_,
           linkset & allLinks_,
           tagset  & allTags_,
           listset & allLists_,
           const std::string & _sentencePath,
           const std::string & _linksPath,
           const std::string & _tagPath,
           const std::string & _listPath );

/**@brief Destroys the parser
 * @return EXIT_SUCCESS on success */
int terminate();

NAMESPACE_END

#endif // TATOPARSER_INTERFACE_LIB_H
