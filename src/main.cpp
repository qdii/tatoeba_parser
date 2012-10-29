#include "prec.h"
#include "dataset.h"
#include "linkset.h"
#include "tagset.h"
#include "options.h"
#include "filter_id.h"
#include "filter_tag.h"
#include "filter_regex.h"
#include "fast_sentence_parser.h"
#include "fast_link_parser.h"
#include "fast_tag_parser.h"
#include "file_mapper.h"
#include "interface_lib.h"
#include <iostream>

USING_NAMESPACE

static const char SENTENCES_FILENAME[] = "sentences.csv";
static const char LINKS_FILENAME[] = "links.csv";
static const char TAG_FILENAME[] = "tags.csv";

void startLog( bool _verbose );

/// How does it work?
///
/// Basically, we check the options the user set through the different program
/// parameters (like "--language fra"), then we parse the different csv files,
/// we create a list of filters and we filter all the sentences based on
/// those filters.

int main( int argc, char * argv[] )
{
    startLog( false );
    FilterVector allFilters; ///< the filter list
    ///< to select sentences based on the user options)

    // create the filters with respect to the user options
    allFilters.reserve( 5 );
    userOptions options;
    try
    {
        options.treatCommandLine( argc, argv );
    }
    catch (const boost::program_options::unknown_option & err)
    {
        qlog::error << "Unknown option: " << err.get_option_name() << '\n';
        return EXIT_FAILURE;
    }
    startLog( options.isVerbose() );
    options.treatConfigFile();
    const std::string csvPath = options.getCsvPath();

    linkset allLinks;
    tagset allTags;
    dataset allSentences; ///< data will contain the sentences

    try
    {
        options.getFilters( allSentences, allLinks, allTags, allFilters );
    }
    catch( const boost::regex_error & err )
    {
        qlog::error << "Invalid regular expression\n"
                    << qlog::setBashColor() << err.what() << '\n';
        return EXIT_FAILURE;
    }

    if( options.isVersionRequested() )
    {
        options.printVersion();
        return EXIT_FAILURE;
    }

    if(      argc == 1
        || ( argc == 2 && !options.justParse() )
        || options.isHelpRequested()            )
    {
        options.printHelp();
        return EXIT_FAILURE;
    }

    // call the parsing library
    init(
            ( options.isItNecessaryToParseLinksFile() ? 0 : NO_LINKS )
        |   ( options.isItNecessaryToParseTagFile() ? 0 : NO_TAGS )
    );

    parse( allSentences, allLinks, allTags, csvPath + '/' + SENTENCES_FILENAME,
            csvPath + '/' + LINKS_FILENAME, csvPath + '/' + TAG_FILENAME );


    ///////////////////////////
    //  filtering sentences  //
    ///////////////////////////
    if( !options.justParse() )
    {
        // go through every sentence and see if it matches the filter
        auto itr = allSentences.begin();
        auto endFilter = allFilters.end();
        unsigned printedLineNumber = 0;
        std::string translationLanguage = options.getFirstTranslationLanguage();
        const std::string & separator = options.getSeparator();
        bool shouldDisplay = true;

        for( sentence sentence : allSentences )
        {
            if (sentence.getId() == sentence::INVALID_ID)
                continue;

            shouldDisplay = true;

            for( auto filter = allFilters.begin(); shouldDisplay && filter != endFilter; ++filter )
            {
                shouldDisplay &= ( *filter )->parse( sentence );
            }

            // display the sentence if it is seleted by all the filters
            if( shouldDisplay )
            {
                // option -n
                if( options.displayLineNumbers() )
                    std::cout << ++printedLineNumber << separator;

                // option -i
                if( options.displayIds() )
                    std::cout << sentence.getId() << separator;

                // display the sentence
                std::cout << sentence.str();

                // display a translation if it was requested
                if ( options.displayFirstTranslation() )
                {
                    // find a suitable translation
                    const sentence::id firstTranslationId =
                        getFirstSentenceTranslation(
                            allSentences,
                            allLinks,
                            sentence.getId(),
                            translationLanguage
                        );

                    if ( firstTranslationId != sentence::INVALID_ID
                            && allSentences[firstTranslationId] )
                    {
                        // display the translation
                        std::cout << separator << allSentences[firstTranslationId]->str();
                    }
                }

                std::cout << '\n';
            }
        }
    }

    terminate();

    return EXIT_SUCCESS;
}

// -------------------------------------------------------------------------- //

void startLog( bool verbose )
{
    qlog::setLogLevel( verbose ? qlog::Loglevel::info : qlog::Loglevel::error );
    qlog::setPrependedTextQdiiFlavourBashColors();
    qlog::setOutput( std::cout );
}
