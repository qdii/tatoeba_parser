#include "prec_library.h"
#include "tatoparser/interface_lib.h"
#include "tatoparser/dataset.h"
#include "tatoparser/tagset.h"
#include "tatoparser/linkset.h"
#include "datainfo.h"
#include "fast_sentence_adv_parser.h"
#include "fast_sentence_parser.h"
#include "fast_link_parser.h"
#include "fast_tag_parser.h"
#include "file_mapper.h"

NAMESPACE_START

static ParserFlag                   g_parserFlags = 0;
static std::unique_ptr<fileMapper>  g_sentenceMap = nullptr;

// -------------------------------------------------------------------------- //

static
bool isFlagSet( ParserFlag _flag )
{
    return g_parserFlags & _flag;
}

// -------------------------------------------------------------------------- //

int init( ParserFlag _flags )
{
    assert( g_parserFlags == 0 );

    g_parserFlags = _flags;

    llog::setOutput( std::cerr );
    llog::setLogLevel( isFlagSet( VERBOSE ) ? llog::Loglevel::info : llog::Loglevel::error );

    return EXIT_SUCCESS;
}

// -------------------------------------------------------------------------- //

template<typename T, typename ...Args> static
std::unique_ptr<T> make_unique( Args&& ...args )
{
    return std::unique_ptr<T>( new T( std::forward<Args>(args)... ) );
}

// -------------------------------------------------------------------------- //

static
std::unique_ptr<fileMapper> mapFileToMemory( const std::string & _file )
{
    std::unique_ptr<fileMapper>  ret = nullptr;

    try
    {
        ret = make_unique<fileMapper>( _file );
    }
    catch( const invalid_file & exception )
    {
        llog::error << "Cannot open " << exception.m_filename << '\n';
    }
    catch( const map_failed & exception )
    {
        llog::error << "Failed to map " << _file << '\n';
    }
    catch( const std::bad_alloc & )
    {
        llog::error << "Out of memory\n";
    }

    return ret;
}

// -------------------------------------------------------------------------- //

static
int parseSentences( const std::string & _sentencesPath, datainfo & _info_, dataset & allSentences_ )
{
    int ret = EXIT_FAILURE;

    // map "sentences.csv" to some address in our virtual space
    g_sentenceMap = std::move( mapFileToMemory( _sentencesPath ) );

    if( g_sentenceMap != nullptr )
    {
        // create the parser
        fastSentenceParser<char *> sentenceParser(
            g_sentenceMap->begin(),
            g_sentenceMap->end()
        );

        // allocate memory for the sentence structure
        _info_.m_nbSentences = sentenceParser.countLinesFast();

        if( _info_.m_nbSentences <= 0 )
        {
            llog::error << _sentencesPath << " is empty\n";
            return ret;
        }

        try
        {
            allSentences_.allocate( _info_ );
        }
        catch( const std::bad_alloc & )
        {
            llog::error << "Not enough memory.\n";
            return ret;
        }

        _info_.m_nbSentences = sentenceParser.start( allSentences_ );

        // retrieve the sentence of highest id so as to be able to create containers
        // of the right size to store links and tags
        const sentence & sentenceOfHighestId =
            *std::max_element(
                allSentences_.begin(), allSentences_.end(),
        []( const sentence & _a, const sentence & _b ) { return _a.getId() < _b.getId(); }
            );

        llog::info << "highest id: " << sentenceOfHighestId.getId() << '\n';
        _info_.m_highestId = sentenceOfHighestId.getId();

        ret = EXIT_SUCCESS;
    }

    return ret;
}

// -------------------------------------------------------------------------- //

static
int parseLinks( const std::string & _linksPath, datainfo & _info_, linkset & allLinks_ )
{
    int ret = EXIT_FAILURE;

    // we map tags.csv to somewhere in our virtual space
    std::unique_ptr<fileMapper> linksMap = std::move( mapFileToMemory( _linksPath ) );

    if( linksMap != nullptr )
    {
        try
        {
            // we create the parser
            fastLinkParser<char *> linkParser( linksMap->begin(), linksMap->end() );

            // we allocate memory for the structure that will store the links
            _info_.m_nbLinks = linkParser.countLines();
            allLinks_.allocate( _info_ );

            // we parse the file and store the data
            linkParser.start( allLinks_ );

            ret = EXIT_SUCCESS;
        }
        catch( const std::bad_alloc & exception )
        {
            llog::error << "Out of memory\n";
        }
    }

    return ret;
}

// -------------------------------------------------------------------------- //

static
int parseTags( const std::string & _tagPath, datainfo &, tagset & allTags_ )
{
    int ret = EXIT_FAILURE;

    std::unique_ptr<fileMapper> tagMap =
        std::move( mapFileToMemory( _tagPath ) );

    if( tagMap != nullptr )
    {
        try
        {
            fastTagParser<char *> tagParser( tagMap->begin(), tagMap->end() );
            tagParser.start( allTags_ );
            ret = EXIT_SUCCESS;
        }
        catch( const std::bad_alloc & )
        {
            llog::error << "An error occurred while parsing file " << _tagPath << std::endl;
        }
    }

    return ret;
}

// -------------------------------------------------------------------------- //

static
int parseDetailed( const std::string & _sentencesPath, datainfo & _info_, dataset & allSentences_ )
{
    int ret = EXIT_FAILURE;

    // map "sentences_detailed.csv" to some address in our virtual space
    g_sentenceMap = std::move( mapFileToMemory( _sentencesPath ) );

    if( g_sentenceMap != nullptr )
    {
        // create the parser
        fastDetailedParser<char *> detailedParser( g_sentenceMap->begin(), g_sentenceMap->end() );

        // allocate memory for the sentence structure
        _info_.m_nbSentences = detailedParser.countLinesFast();

        if( _info_.m_nbSentences <= 0 )
        {
            llog::error << _sentencesPath << " is empty\n";
            return ret;
        }

        try
        {
            allSentences_.allocate( _info_ );
        }
        catch( const std::bad_alloc & )
        {
            llog::error << "Not enough memory.\n";
            return ret;
        }

        _info_.m_nbSentences = detailedParser.start( allSentences_ );

        // retrieve the sentence of highest id so as to be able to create containers
        // of the right size to store links and tags
        const sentence & sentenceOfHighestId =
            *std::max_element(
                allSentences_.begin(), allSentences_.end(),
                []( const sentence & _a, const sentence & _b ) { return _a.getId() < _b.getId(); }
            );

        llog::info << "highest id: " << sentenceOfHighestId.getId() << '\n';
        _info_.m_highestId = sentenceOfHighestId.getId();

        ret = EXIT_SUCCESS;
    }

    return ret;
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
        parsingSuccess = isFlagSet( DETAILED ) ?
                         parseDetailed( sentencePath, info, allSentences_ ):
                         parseSentences( sentencePath, info, allSentences_ );

        if ( parsingSuccess == EXIT_SUCCESS )
        {
            // create an container to retrieve sentences from id in a very fast manner
            try
            {
                allSentences_.prepare( info );
            }
            catch( const std::bad_alloc & exc )
            {
                llog::error << "Not enough memory.\n";
                parsingSuccess = EXIT_FAILURE;
            }
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
    g_sentenceMap = nullptr;
    g_parserFlags = 0;

    return EXIT_SUCCESS;
}

NAMESPACE_END
