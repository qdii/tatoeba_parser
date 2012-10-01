#include "prec.h"
#include "dataset.h"
#include "options.h"
#include "filter_id.h"
#include "filter_tag.h"
#include "filter_regex.h"
#include "fast_sentence_parser.h"
#include "fast_link_parser.h"
#include "fast_tag_parser.h"
#include <iostream>

USING_NAMESPACE

static const char SENTENCES_FILENAME[] = "sentences.csv";
static const char LINKS_FILENAME[] = "links.csv";
static const char TAG_FILENAME[] = "tags.csv";

void startLog( bool _verbose );
int parseFile( parser<char *> & _parser, dataset & data_ );
bool parseTagFile( fileMapper *& _filemap, const std::string & _filename, dataset & _data );

/** @brief parse the sentences.csv file and returns the nb of lines */
int parseSentenceFile();

/// How does it work?
///
/// Basically, we check the options the user set through the different program
/// parameters (like "--language fra"), then we parse the different csv files,
/// we create a list of filters and we filter all the sentences based on
/// those filters.

int main( int argc, char * argv[] )
{
    FilterVector allFilters; ///< the filter list
    ///< to select sentences based on the user options)

    // create the filters with respect to the user options
    allFilters.reserve( 5 );
    userOptions options;
    options.treatCommandLine( argc, argv );
    startLog( options.isVerbose() );

    const std::string csvPath = options.getCsvPath();

    dataset data; ///< data will contain the sentences and how they are linked

    try
    {
        options.getFilters( data, allFilters );
    }
    catch( const boost::regex_error & err )
    {
        qlog::error << "Invalid regular expression\n"
                    << qlog::setBashColor() << err.what() << '\n';
        return 0;
    }

    if( options.isVersionRequested() )
    {
        options.printVersion();
        return 0;
    }

    if( ( !allFilters.size() && !options.justParse() ) || options.isHelpRequested() )
    {
        options.printHelp();
        return 0;
    }

    // parsing sentences file
    fileMapper * sentenceMap = nullptr;
    const std::string sentencesPath = csvPath + '/' + SENTENCES_FILENAME;

    try
    {
        sentenceMap = new fileMapper( sentencesPath );
    }
    catch( const invalid_file & exception )
    {
        qlog::error << "Cannot open " << sentencesPath << '\n';
        return 0;
    }
    catch( const map_failed & exception )
    {
        qlog::error << "Failed to map " << sentencesPath << '\n';
        return 0;
    }

    assert( sentenceMap );
    assert( sentenceMap->getRegion() );

    data.allocateMemoryForSentences(
        std::count(
            sentenceMap->getRegion(),
            sentenceMap->getRegion() + sentenceMap->getSize(), '\n'
        )
    );

    fastSentenceParser<char *> sentenceParser(
        sentenceMap->getRegion(),
        sentenceMap->getRegion() + sentenceMap->getSize()
    );
    const int nbLines = parseFile( sentenceParser, data );

    if( nbLines < 0 )
        return 0;

    const sentence & sentenceOfHighestId =
        *std::max_element(
            data.begin(), data.end(),
            []( const sentence & _a, const sentence & _b ) { return _a.getId() < _b.getId(); }
        );

    qlog::info << "highest id: " << sentenceOfHighestId.getId() << '\n';

    // parsing links.csv
    if( options.isItNecessaryToParseLinksFile() )
    {
        const std::string linksPath = csvPath + '/' + LINKS_FILENAME;

        try
        {
            fileMapper linksMap( linksPath );
            const size_t nbLinks = std::count(
                                       linksMap.getRegion(), linksMap.getRegion() + linksMap.getSize(),
                                       '\n'
                                   );
            data.allocateMemoryForLinks( nbLinks, sentenceOfHighestId.getId() );
            fastLinkParser<char *> linksParser(
                linksMap.getRegion(), linksMap.getRegion() + linksMap.getSize()
            );
            parseFile( linksParser, data );
        }
        catch( const invalid_file & exception )
        {
            qlog::error << "Cannot open " << linksPath << '\n';
            return 0;
        }
        catch( const map_failed & exception )
        {
            qlog::error << "Failed to map " << linksPath << '\n';
            return 0;
        }
    }

    // parsing tags.csv
    fileMapper * tagFileMapping = nullptr;

    if( options.isItNecessaryToParseTagFile() )
    {
        const std::string tagsPath = csvPath + '/' + TAG_FILENAME;

        if( !parseTagFile( tagFileMapping, tagsPath, data ) )
            return 0;
    }

    if( !options.justParse() )
    {
        // create an container to retrieve sentences from id in a very fast manner
        data.prepare();

        // go through every sentence and see if it matches the filter
        auto itr = data.begin();
        auto endFilter = allFilters.end();
        unsigned printedLineNumber = 0;

        for( int i = 0; i < nbLines; ++i )
        {
            auto sentence = *itr++;
            bool shouldDisplay = true;

            for( auto filter = allFilters.begin(); shouldDisplay && filter != endFilter; ++filter )
            {
                shouldDisplay &= ( *filter )->parse( sentence );
            }

            if( shouldDisplay )
            {
                if( options.displayLineNumbers() )
                    std::cout << ++printedLineNumber << '\t';

                if( options.displayIds() )
                    std::cout << sentence.getId() << '\t';

                std::cout << sentence.str() << '\n';
            }
        }

        delete sentenceMap;

    }

    return 0;
}

// -------------------------------------------------------------------------- //

bool parseTagFile( fileMapper *& _filemap, const std::string & _filename, dataset & _data )
{
    try
    {
        _filemap = new fileMapper( _filename );
        const size_t nbLines =
            std::count(
                _filemap->getRegion(),
                _filemap->getRegion() + _filemap->getSize(),
                '\n'
            );

        _data.allocateMemoryForTags( nbLines );
        fastTagParser<char *> parser(
            _filemap->getRegion(), _filemap->getRegion() + _filemap->getSize()
        );
        parser.setOutput( _data );
        parser.start();
    }
    catch( const std::bad_alloc & exc )
    {
        qlog::error << "An error occurred while parsing file " << _filename << std::endl;

        return false;
    }
    catch( const invalid_file & exception )
    {
        qlog::error << "Cannot open " << _filename << '\n';
        return false;
    }
    catch( const map_failed & exception )
    {
        qlog::error << "Failed to map " << _filename << '\n';
        return false;

    }

    return true;
}

// -------------------------------------------------------------------------- //

int parseFile( parser<char *> & _parser, dataset & data_ )
{
    _parser.setOutput( data_ );
    return _parser.start();
}

// -------------------------------------------------------------------------- //

void startLog( bool verbose )
{
    qlog::setLogLevel( verbose ? qlog::Loglevel::info : qlog::Loglevel::error );
    qlog::setPrependedTextQdiiFlavourBashColors();
    qlog::setOutput( std::cout );
}
