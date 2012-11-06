#include "prec_library.h"
#include "tatoparser/interface_lib.h"
#include "tatoparser/dataset.h"
#include "tatoparser/tagset.h"
#include "tatoparser/linkset.h"
#include "datainfo.h"
#include "fast_sentence_parser.h"
#include "fast_link_parser.h"
#include "fast_tag_parser.h"
#include "file_mapper.h"

NAMESPACE_START

static ParserFlag g_parserFlags = 0;

static fileMapper * g_sentenceMap = nullptr;
static fileMapper * g_linksMap = nullptr;
static fileMapper * g_tagMap = nullptr;

static fastSentenceParser<char *> * g_sentenceParser = nullptr;
static fastTagParser<char *> *      g_tagParser = nullptr;
static fastLinkParser<char *> *     g_linkParser = nullptr;

// -------------------------------------------------------------------------- //

static
bool isFlagSet( ParserFlag _flag )
{
    return g_parserFlags & _flag;
}

// -------------------------------------------------------------------------- //

int init( ParserFlag _flags )
{
    assert( g_sentenceMap == nullptr );
    assert( g_linksMap == nullptr );
    assert( g_tagMap == nullptr );
    assert( g_parserFlags == 0 );
    assert( g_sentenceParser == nullptr );
    assert( g_tagParser == nullptr );
    assert( g_linkParser == nullptr );

    g_parserFlags = _flags;

    llog::setOutput(std::cerr);
    llog::setLogLevel( isFlagSet(VERBOSE) ? llog::Loglevel::info : llog::Loglevel::error );

    return EXIT_SUCCESS;
}

// -------------------------------------------------------------------------- //

static
int parseSentences( const std::string & _sentencesPath, datainfo & _info_, dataset & allSentences_ )
{
    // map "sentences.csv" to some address in our virtual space
    try
    {
        g_sentenceMap = new fileMapper( _sentencesPath );
    }
    catch( const invalid_file & exception )
    {
        llog::error << "Cannot open " << _sentencesPath << '\n';
        return EXIT_FAILURE;
    }
    catch( const map_failed & exception )
    {
        llog::error << "Failed to map " << _sentencesPath << '\n';
        return EXIT_FAILURE;
    }

    // create the parser
    g_sentenceParser = new fastSentenceParser<char *>(
        g_sentenceMap->begin(),
        g_sentenceMap->end()
    );

    // allocate memory for the sentence structure
    _info_.m_nbSentences = g_sentenceParser->countLinesFast();

    if( _info_.m_nbSentences <= 0 )
    {
        llog::error << _sentencesPath << " is empty\n";
        return EXIT_FAILURE;
    }

    try
    {
        allSentences_.allocate( _info_ );
    }
    catch( const std::bad_alloc & )
    {
        llog::error << "Not enough memory.\n";
        return EXIT_FAILURE;
    }

    _info_.m_nbSentences = g_sentenceParser->start( allSentences_ );

    // retrieve the sentence of highest id so as to be able to create containers
    // of the right size to store links and tags
    const sentence & sentenceOfHighestId =
        *std::max_element(
            allSentences_.begin(), allSentences_.end(),
            []( const sentence & _a, const sentence & _b ) { return _a.getId() < _b.getId(); }
        );

    llog::info << "highest id: " << sentenceOfHighestId.getId() << '\n';
    _info_.m_highestId = sentenceOfHighestId.getId();

    return EXIT_SUCCESS;
}

// -------------------------------------------------------------------------- //

static
int parseLinks( const std::string & _linksPath, datainfo & _info_, linkset & allLinks_ )
{
    try
    {
        // we map tags.csv to somewhere in our virtual space
        fileMapper linksMap( _linksPath );

        // we create the parser
        g_linkParser = new fastLinkParser<char *>( linksMap.begin(), linksMap.end() );

        // we allocate memory for the structure that will store the links
        _info_.m_nbLinks = g_linkParser->countLines();
        allLinks_.allocate( _info_ );

        // we parse the file and store the data
        g_linkParser->start( allLinks_ );
    }
    catch( const invalid_file & exception )
    {
        llog::error << "Cannot open " << _linksPath << '\n';
        return EXIT_FAILURE;
    }
    catch( const map_failed & exception )
    {
        llog::error << "Failed to map " << _linksPath << '\n';
        return EXIT_FAILURE;
    }
    catch( const std::bad_alloc & exception )
    {
        llog::error << "Out of memory\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

// -------------------------------------------------------------------------- //

static
int parseTags( const std::string & _tagPath, datainfo &, tagset & allTags_ )
{
    try
    {
        g_tagMap = new fileMapper( _tagPath );
        g_tagParser = new fastTagParser<char *>(
            g_tagMap->begin(),
            g_tagMap->end()
        );
        g_tagParser->start( allTags_ );
    }
    catch( const std::bad_alloc & )
    {
        llog::error << "An error occurred while parsing file " << _tagPath << std::endl;
        return EXIT_FAILURE;
    }
    catch( const invalid_file & )
    {
        llog::error << "Cannot open " << _tagPath << '\n';
        return EXIT_FAILURE;
    }
    catch( const map_failed & )
    {
        llog::error << "Failed to map " << _tagPath << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

// -------------------------------------------------------------------------- //

int  parse( dataset & allSentences_,
            linkset & allLinks_,
            tagset  & allTags_,
            const std::string & sentencePath,
            const std::string & linksPath,
            const std::string & tagPath )
{
    datainfo info;
    int parsingSuccess = EXIT_SUCCESS;

    if( sentencePath.size() )
    {
        parsingSuccess = parseSentences( sentencePath, info, allSentences_ );

        // create an container to retrieve sentences from id in a very fast manner
        try
        {
            allSentences_.prepare( info );
        }
        catch( const std::bad_alloc & exc)
        {
            llog::error << "Not enough memory.\n";
            return EXIT_FAILURE;
        }
    }

    if( parsingSuccess != EXIT_FAILURE && linksPath.size() && !isFlagSet( NO_LINKS ) )
        parsingSuccess = parseLinks( linksPath, info, allLinks_ );

    if( parsingSuccess != EXIT_FAILURE && tagPath.size() && !isFlagSet( NO_TAGS ) )
        parsingSuccess = parseTags( tagPath, info, allTags_ );

    return parsingSuccess;
}

// -------------------------------------------------------------------------- //

int terminate()
{
    delete g_sentenceMap;           g_sentenceMap = nullptr;
    delete g_linksMap;              g_linksMap = nullptr;
    delete g_tagMap;                g_tagMap = nullptr;
    delete g_sentenceParser;        g_sentenceParser = nullptr;
    delete g_tagParser;             g_tagParser = nullptr;
    delete g_linkParser;            g_linkParser = nullptr;

    g_parserFlags = 0;

    return EXIT_SUCCESS;
}

NAMESPACE_END
