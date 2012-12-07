#include "prec.h"
#include <tatoparser/dataset.h>
#include <tatoparser/linkset.h>
#include <tatoparser/tagset.h>
#include <tatoparser/interface_lib.h>
#include "options.h"
#include "filter_id.h"
#include "filter_tag.h"
#include "filter_regex.h"
#include <iostream>

USING_NAMESPACE

static const char SENTENCES_FILENAME[] = "sentences.csv";
static const char DETAILED_FILENAME[] = "sentences_detailed.csv";
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
	qlog::init();
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
    catch( const boost::program_options::unknown_option & err )
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
                    << qlog::color() << err.what() << '\n';
        return EXIT_FAILURE;
    }

    if( options.isVersionRequested() )
    {
        options.printVersion();
        return EXIT_FAILURE;
    }

    if( argc == 1
            || ( argc == 2 && !options.justParse() )
            || options.isHelpRequested() )
    {
        options.printHelp();
        return EXIT_FAILURE;
    }

    // call the parsing library
    const int libraryInit =
        init(
            ( options.isItNecessaryToParseLinksFile() ? 0 : NO_LINKS ) |
            ( options.isItNecessaryToParseTagFile() ? 0 : NO_TAGS ) |
            ( options.isVerbose() ? VERBOSE : 0 ) |
            ( options.isItNecessaryToParseDetailedFile() ? DETAILED : 0 )
	        );

    if (libraryInit != EXIT_SUCCESS)
        return EXIT_FAILURE;

    const int libraryParsing =
        parse( allSentences, allLinks, allTags,
               options.isItNecessaryToParseDetailedFile() ?
                    csvPath + '/' + DETAILED_FILENAME     :
                    csvPath + '/' + SENTENCES_FILENAME,
           csvPath + '/' + LINKS_FILENAME, csvPath + '/' + TAG_FILENAME );

    if (libraryParsing != EXIT_SUCCESS)
        return EXIT_FAILURE;


    ///////////////////////////
    //  filtering sentences  //
    ///////////////////////////
    if( !options.justParse() )
    {
        // go through every sentence and see if it matches the filter
        auto endFilter = allFilters.end();
        unsigned printedLineNumber = 0;
        std::string translationLanguage = options.getFirstTranslationLanguage();
        const std::string & separator = options.getSeparator();
        bool shouldDisplay = true;

        for( sentence sentence : allSentences )
        {
            if( sentence.getId() == sentence::INVALID_ID )
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
                if( options.displayFirstTranslation() )
                {
                    // find a suitable translation
                    const sentence::id firstTranslationId =
                        getFirstSentenceTranslation(
                            allSentences,
                            allLinks,
                            sentence.getId(),
                            translationLanguage
                        );

                    if( firstTranslationId != sentence::INVALID_ID
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

    tato::terminate();
	qlog::destroy();

    return EXIT_SUCCESS;
}

// -------------------------------------------------------------------------- //

void startLog( bool verbose )
{
	using namespace qlog;

	qlog::set_loglevel( verbose ? qlog::loglevel::info : qlog::loglevel::error );

	qlog::info.reset_decoration();
	qlog::warning.reset_decoration();
	qlog::error.reset_decoration();

    // prepend markers
    qlog::info.prepend() << "[..] ";
    qlog::warning.prepend() << "[" << color(green) << "ww" << color() << "] ";
    qlog::error.prepend() << "[" << color(red, true) << "EE" << color() << "] " << color(white, true);

    // append just resets color
    qlog::error.append() << color();

    qlog::set_output( std::cerr );
}
