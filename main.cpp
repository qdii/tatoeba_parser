#include "prec.h"
#include "dataset.h"
#include "options.h"
#include "filter_id.h"
#include "filter_regex.h"
#include "fast_sentence_parser.h"
#include "fast_link_parser.h"
#include <iostream>

USING_NAMESPACE

static const char SENTENCES_FILENAME[] = "sentences.csv";
static const char LINKS_FILENAME[] = "links.csv";
void startLog( bool _verbose );
int parseFile( parser & _parser, dataset & data_ );

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

    if( !allFilters.size() || options.isHelpRequested() )
    {
        options.printHelp();
        return 0;
    }

    // parsing sentences file
    fileMapper * sentenceMap = nullptr;

    try
    {
        sentenceMap = new fileMapper( SENTENCES_FILENAME );
    }
    catch( const invalid_file & exception )
    {
        qlog::error << "Cannot open " << SENTENCES_FILENAME << '\n';
        return 0;
    }
    catch( const map_failed & exception )
    {
        qlog::error << "Failed to map " << SENTENCES_FILENAME << '\n';
        return 0;
    }

    assert( sentenceMap );
    assert( sentenceMap->getRegion() );

    data.allocateMemoryForSentences( std::count(sentenceMap->getRegion(), sentenceMap->getRegion() + sentenceMap->getSize(), '\n') );

    fastSentenceParser sentenceParser(
        sentenceMap->getRegion(),
        sentenceMap->getRegion() + sentenceMap->getSize()
    );
    const int nbLines = parseFile( sentenceParser, data );

    if( nbLines < 0 )
        return 0;

    qlog::info << "higher id: " << (std::max_element(
        data.begin(), data.end(),
        [](const sentence & _a, const sentence & _b) { return _a.getId() < _b.getId(); }))->getId() << '\n';

    // parsing links.csv
    if( options.isItNecessaryToParseLinksFile() )
    {
        try
        {
            fileMapper linksMap( LINKS_FILENAME );
            const size_t nbLinks = std::count ( linksMap.getRegion(), linksMap.getRegion() + linksMap.getSize(), '\n' );
            data.allocateMemoryForLinks( nbLinks );
            fastLinkParser linksParser( linksMap.getRegion(), linksMap.getRegion() + linksMap.getSize() );
            parseFile( linksParser, data );
        }
        catch( const invalid_file & exception )
        {
            qlog::error << "Cannot open " << LINKS_FILENAME << '\n';
            return 0;
        }
        catch( const map_failed & exception )
        {
            qlog::error << "Failed to map " << LINKS_FILENAME << '\n';
            return 0;
        }
    }

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

    return 0;
}


int parseFile( parser & _parser, dataset & data_ )
{
    _parser.setOutput( data_ );
    return _parser.start();
}

void startLog( bool verbose )
{
    qlog::setLogLevel( verbose ? qlog::Loglevel::info : qlog::Loglevel::error );
    qlog::setPrependedTextQdiiFlavourBashColors();
    qlog::setOutput( std::cout );
}
