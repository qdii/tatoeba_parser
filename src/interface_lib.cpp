#include "prec_library.h"
#include "tatoparser/interface_lib.h"
#include "tatoparser/dataset.h"
#include "tatoparser/tagset.h"
#include "tatoparser/linkset.h"
#include "tatoparser/listset.h"
#include "datainfo.h"
#include "fast_sentence_adv_parser.h"
#include "fast_sentence_parser.h"
#include "fast_link_parser.h"
#include "fast_list_parser.h"
#include "fast_tag_parser.h"
#include "file_mapper.h"

NAMESPACE_START

#pragma GCC visibility push(hidden)

// -------------------------------------------------------------------------- //

static ParserFlag                   g_parserFlags = 0;
static std::unique_ptr<fileMapper>  g_sentenceMap = nullptr;

static fastDetailedParser<char *>*   g_detailedParser = nullptr;
static fastLinkParser<char *>*       g_fastLinkParser = nullptr;
static fastSentenceParser<char *>*   g_sentenceParser = nullptr;
static fastSentenceParser<char *>*   g_sentenceParserParallel = nullptr;
static fastTagParser<char *>*        g_tagParser = nullptr;
static fastListParser<char *>*       g_listParser = nullptr;

// -------------------------------------------------------------------------- //

static bool                         g_quit = false; // leave on abort

// -------------------------------------------------------------------------- //

static
bool isFlagSet( ParserFlag _flag )
{
    return ( g_parserFlags & _flag ) != 0;
}


// -------------------------------------------------------------------------- //

#ifdef TATO_USE_VARIADIC_TEMPLATES
template<typename T, typename ...Args> static
std::unique_ptr<T> make_unique( Args&& ...args )
{
    return std::unique_ptr<T>( new T( std::forward<Args>( args )... ) );
}
#else
template<typename T, typename Arg> static
std::unique_ptr<T> make_unique( Arg && arg )
{
    return std::unique_ptr<T>( new T( std::forward<Arg>( arg ) ) );
}
#endif
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
    catch( const map_failed & )
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
// This function treats half a deck, when the parser is run in multi-core mod
// it returns the nb of lines which have been parsed and the id of the senten
// which id was the highest
static
std::pair<size_t, sentence::id> treatHalf(
     char * const begin, char * const end, dataset & allSentences_)
{
    fastSentenceParser<char *> parser( begin, end );

    // this is ugly and buggy but that's okay: the race condition it can creat
    // on extreme cases will just cause the program to take an extra 0.5 s to sto
    if (g_sentenceParser)
        g_sentenceParserParallel = &parser;
    else
        g_sentenceParser = &parser;

    const size_t nbLinesParsed  = parser.start( allSentences_ );
    const sentence & sentenceOfHighestId =
        *std::max_element(
                allSentences_.begin(), allSentences_.end(),
                []( const sentence & _a, const sentence & _b ) { return _a.getId() < _b.getId(); }
        );

    if (g_sentenceParser == &parser)
        g_sentenceParser = nullptr;
    else if (g_sentenceParserParallel == &parser)
        g_sentenceParserParallel = nullptr;

    return std::pair<size_t, sentence::id>( nbLinesParsed, sentenceOfHighestId.getId() );
}

static
int parseSentencesParallel( const std::string & _sentencesPath, datainfo & _info_, dataset & allSentences_ )
{
    llog::info << "starting parallel parsing\n";
    g_sentenceMap = mapFileToMemory( _sentencesPath );
    if( g_sentenceMap == nullptr )
        return EXIT_FAILURE;

    // we want to know the number of lines the file contains
    // to split the work load into different threads. We cannot
    // just split the file in the middle as we might break a
    // sentence in two parts.
    size_t estimatedNbLines = 0;
    {
        fastSentenceParser<const char *> lineCounter(
            g_sentenceMap->begin(),
            g_sentenceMap->end()
        );
        estimatedNbLines = lineCounter.countLinesFast();
        llog::info << "estimated number of sentences: " << estimatedNbLines << '\n';
    }

    // if the file is empty, then we have nothing to do.
    if (estimatedNbLines == 0)
    {
        llog::warning << _sentencesPath << " is empty.\n";
        return EXIT_SUCCESS;
    }

    // we need to find the position just after the first
    // half of the set of sentences.
    char * splitPosition = reinterpret_cast<char*>( (reinterpret_cast<uint64_t>(g_sentenceMap->begin())/2) + (reinterpret_cast<uint64_t>(g_sentenceMap->end())/ 2) );

    // we adjust the position so that it falls after the end of a line
    while( *splitPosition++ != '\n' )
    {
        assert( splitPosition != g_sentenceMap->end() );
    }
    llog::info << "split pos: " << static_cast<void*>(splitPosition) << '\n';

    // when we arrive here, we should be just after a '\n' character, otherwi
    // we are in the middle of a senten
    assert( *(splitPosition-1) == '\n' );

    // we need a dataset so that each thread writes in its own memory space.
    dataset secondHalfSentences;
    secondHalfSentences.allocate( estimatedNbLines / 2 );

    // allSentences_ will actually contain all the lines in the end
    allSentences_.allocate( 2 * estimatedNbLines );

    std::future< std::pair< size_t, sentence::id > > futureResultTop =
        std::async( std::launch::async, treatHalf, g_sentenceMap->begin(), splitPosition, std::ref(allSentences_) );

    std::pair< size_t, sentence::id > resultBottom = treatHalf( splitPosition, g_sentenceMap->end(), std::ref(secondHalfSentences) );

    // in addition to computing the highest id, this will ensure that
    // the two threads are done executing
    std::pair<size_t, sentence::id> resultTop = futureResultTop.get();

    const sentence::id highestIdOfTop = resultTop.second;
    const sentence::id highestIdOfBottom = resultBottom.second;
    _info_.m_highestId = highestIdOfTop > highestIdOfBottom ? highestIdOfTop : highestIdOfBottom;
    llog::info << "highest id: " << _info_.m_highestId << '\n';

    _info_.m_nbSentences = resultTop.first + resultBottom.first;
    llog::info << "parsed " << _info_.m_nbSentences << "sentences.\n";

    // concatenate both containers
    allSentences_.merge( std::move( secondHalfSentences ));

    return EXIT_SUCCESS;
}

static
int parseSentences( const std::string & _sentencesPath, datainfo & _info_, dataset & allSentences_ )
{
    int ret = EXIT_FAILURE;

    // map "sentences.csv" to some address in our virtual space
    g_sentenceMap = mapFileToMemory( _sentencesPath );

    if( g_sentenceMap != nullptr )
    {
        // create the parser
        fastSentenceParser<char *> sentenceParser(
            g_sentenceMap->begin(),
            g_sentenceMap->end()
        );

        // we need to set a global reference to the parser so that a signal can stop it
        // this can happen if the user presses CTRL-C
        g_sentenceParser = &sentenceParser;

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
            g_sentenceParser = nullptr;
            llog::error << "Not enough memory.\n";
            return ret;
        }

        _info_.m_nbSentences = sentenceParser.start( allSentences_ );

        llog::info << "parsed " << _info_.m_nbSentences << "sentences.\n";

        // retrieve the sentence of highest id so as to be able to create containers
        // of the right size to store links and tags
        const sentence & sentenceOfHighestId =
            *std::max_element(
                allSentences_.begin(), allSentences_.end(),
                []( const sentence & _a, const sentence & _b ) { return _a.getId() < _b.getId(); }
            );

        llog::info << "highest id: " << sentenceOfHighestId.getId() << '\n';
        _info_.m_highestId = sentenceOfHighestId.getId();

        g_sentenceParser = nullptr;
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
    std::unique_ptr<fileMapper> linksMap = mapFileToMemory( _linksPath );

    if( linksMap != nullptr )
    {
        try
        {
            // we create the parser
            fastLinkParser<char *> linkParser( linksMap->begin(), linksMap->end() );
            g_fastLinkParser = &linkParser;

            // we allocate memory for the structure that will store the links
            _info_.m_nbLinks = linkParser.countLines();
            allLinks_.allocate( _info_ );

            // we parse the file and store the data
            linkParser.start( allLinks_ );

            g_fastLinkParser = nullptr;
            ret = EXIT_SUCCESS;
        }
        catch( const std::bad_alloc & )
        {
            g_fastLinkParser = nullptr;
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

    std::unique_ptr<fileMapper> tagMap = mapFileToMemory( _tagPath );

    if( tagMap != nullptr )
    {
        try
        {
            fastTagParser<char *> tagParser( tagMap->begin(), tagMap->end() );
            g_tagParser = &tagParser;
            tagParser.start( allTags_ );
            g_tagParser = nullptr;
            ret = EXIT_SUCCESS;
        }
        catch( const std::bad_alloc & )
        {
            g_tagParser =nullptr;
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
    g_sentenceMap = mapFileToMemory( _sentencesPath );

    if( g_sentenceMap != nullptr )
    {
        // create the parser
        fastDetailedParser<char *> detailedParser( g_sentenceMap->begin(), g_sentenceMap->end() );
        g_detailedParser = &detailedParser;

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
            g_detailedParser = nullptr;
            llog::error << "Not enough memory.\n";
            return ret;
        }

        _info_.m_nbSentences = detailedParser.start( allSentences_ );
        g_detailedParser = nullptr;

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
int parseLists( const std::string & _listPath, datainfo & _info, listset & allLists_ )
{
    int ret = EXIT_FAILURE;
    std::unique_ptr<fileMapper> linksMap = mapFileToMemory( _listPath );

    if ( nullptr != linksMap )
    {
        fastListParser<char*> parser( linksMap->begin(), linksMap->end() );
        g_listParser = &parser;
        parser.start( allLists_ );
        g_listParser = nullptr;
        ret = EXIT_SUCCESS;
    }

    return ret;
}

// -------------------------------------------------------------------------- //
static
void startLogging( bool verbose )
{
    using namespace llog;

#   ifdef TATO_ANDROID
    llog::setModuleName("tatoparser");
#   endif

    // prepend markers
    llog::info.prepend() << "[..] ";
    llog::warning.prepend() << "[" << color( green ) << "ww" << color() << "] ";
    llog::error.prepend() << "[" << color( red, true ) << "EE" << color() << "] " << color( white, true );

    // append just resets color
    llog::error.append() << color();

    llog::set_loglevel( verbose ? loglevel::info : loglevel::error );
}

#pragma GCC visibility pop

// -------------------------------------------------------------------------- //

int init( ParserFlag _flags )
{
    assert( g_parserFlags == 0 );

    g_parserFlags = _flags;

    llog::init();
    llog::set_output( std::cerr );

    startLogging( isFlagSet( VERBOSE ) );

    return EXIT_SUCCESS;
}

// -------------------------------------------------------------------------- //

int terminate()
{
    g_sentenceMap = nullptr;
    g_parserFlags = 0;

    llog::destroy();
    return EXIT_SUCCESS;
}

// -------------------------------------------------------------------------- //

int parse_( dataset & allSentences_,
            linkset & allLinks_,
            tagset  & allTags_,
            listset & allLists_,
            const char * _sentencePath,
            const char * _linksPath,
            const char * _tagPath,
            const char * _listPath )
{
    assert( _sentencePath );
    assert( _linksPath );
    assert( _tagPath );
    assert( _listPath );

    const std::string sentencePath( _sentencePath );
    const std::string linksPath( _linksPath );
    const std::string tagPath( _tagPath );
    const std::string listPath( _listPath );

    return parse( allSentences_, allLinks_, allTags_, allLists_, sentencePath, linksPath, tagPath, listPath );
}

// -------------------------------------------------------------------------- //

int parse( dataset & allSentences_,
           linkset & allLinks_,
           tagset  & allTags_,
           listset & allLists_,
           const std::string & _sentencePath,
           const std::string & _linksPath,
           const std::string & _tagPath,
           const std::string & _listPath )
{
    datainfo info; //holds information about the number of sentences, of links, etc.
                   //then dataset and linkset can allocate memory in one shot.

    int parsingSuccess = EXIT_SUCCESS;

    if( _sentencePath.size() )
    {
        parsingSuccess = isFlagSet( DETAILED ) ?
                         parseDetailed( _sentencePath, info, allSentences_ ) :
                            isFlagSet( PARALLEL ) ?
                                parseSentencesParallel( _sentencePath, info, allSentences_ ) :
                                parseSentences        ( _sentencePath, info, allSentences_);
    }

    if( parsingSuccess != EXIT_FAILURE && _linksPath.size() && !isFlagSet( NO_LINKS ) && !g_quit )
        parsingSuccess = parseLinks( _linksPath, info, allLinks_ );

    if( parsingSuccess != EXIT_FAILURE && _tagPath.size() && !isFlagSet( NO_TAGS ) && !g_quit)
        parsingSuccess = parseTags( _tagPath, info, allTags_ );

    if( parsingSuccess != EXIT_FAILURE && _listPath.size() && !isFlagSet( NO_LISTS ) && !g_quit)
        parsingSuccess = parseLists( _listPath, info, allLists_ );

    if( parsingSuccess == EXIT_SUCCESS && !g_quit)
    {
        const sentence::id highestLinkId = allLinks_.getHighestSentenceId();
        if( highestLinkId > info.m_highestId )
            info.m_highestId = highestLinkId;

        // create an container to retrieve sentences from id in a very fast manner
        try
        {
            allSentences_.prepare( info );
        }
        catch( const std::bad_alloc & )
        {
            llog::error << "Not enough memory.\n";
            parsingSuccess = EXIT_FAILURE;
        }
    }

    g_quit = false;

    return parsingSuccess;
}

// -------------------------------------------------------------------------- //

void cancel()
{
    if (g_detailedParser != nullptr) g_detailedParser->abort();
    if (g_fastLinkParser != nullptr) g_fastLinkParser->abort();
    if (g_sentenceParser != nullptr) g_sentenceParser->abort();
    if (g_sentenceParserParallel != nullptr) g_sentenceParserParallel->abort();
    if (g_tagParser      != nullptr) g_tagParser->abort();
    if (g_listParser     != nullptr) g_listParser->abort();

    g_quit = true;
}


NAMESPACE_END
