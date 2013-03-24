#include "prec.h"
#include <tatoparser/dataset.h>
#include <tatoparser/linkset.h>
#include <tatoparser/tagset.h>
#include <tatoparser/listset.h>
#include <tatoparser/interface_lib.h>
#include "options.h"
#include "filter_id.h"
#include "filter_tag.h"
#include "filter_regex.h"
#include "filter_fuzzy.h"
#include <iostream>
#include <fstream>

#ifdef HAVE_CURL_CURL_H
#   include "hdownload.hpp"
#endif

USING_NAMESPACE

static const char SENTENCES_FILENAME[] = "sentences.csv";
static const char DETAILED_FILENAME[] = "sentences_detailed.csv";
static const char LINKS_FILENAME[] = "links.csv";
static const char TAG_FILENAME[] = "tags.csv";
static const char LIST_FILENAME[] = "lists.csv";

static const char DETAILED_URL[] = "http://tatoeba.org/files/downloads/sentences_detailed.csv";
static const char SENTENCES_URL[] = "http://tatoeba.org/files/downloads/sentences.csv";
static const char LINKS_URL[] = "http://tatoeba.org/files/downloads/links.csv";
static const char TAG_URL[] = "http://tatoeba.org/files/downloads/tags.csv";
static const char LIST_URL[] = "http://tatoeba.org/files/downloads/lists.csv";

void startLog( bool _verbose );
void displaySentence( userOptions & _options, dataset & _allSentences, linkset & _allLinks, const sentence & _sentence, unsigned _lineNumber );

#ifdef HAVE_CURL_CURL_H
bool downloadIf( bool _condition, std::string _url, std::string _destinationFile );
#endif

/// How does it work?
///
/// Basically, we check the options the user set through the different program
/// parameters (like "--language fra"), then we parse the different csv files,
/// we create a list of filters and we filter all the sentences based on
/// those filters.

int main( int argc, char * argv[] )
{
    qlog::initializer log_init;
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
    catch( const boost::program_options::invalid_command_line_syntax & err )
    {
        qlog::error << "The parameter " << err.tokens() << " was expecting a value\n";
        return EXIT_FAILURE;
    }
    catch( const boost::program_options::invalid_option_value & err )
    {
        qlog::error << "Invalid parameter value\n";
        return EXIT_FAILURE;
    }


    startLog( options.isVerbose() );
    options.treatConfigFile();
    const std::string csvPath = options.getCsvPath();

    linkset allLinks;
    tagset allTags;
    dataset allSentences; ///< data will contain the sentences
    listset allLists;

    try
    {
        options.getFilters( allSentences, allLinks, allTags, allLists, allFilters );
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
            || ( argc == 2 && !options.justParse() && !options.orphansOnly() )
            || options.isHelpRequested() )
    {
        options.printHelp();
        return EXIT_FAILURE;
    }

    bool skipFiltering = options.justParse();


    // call the parsing library
    const int libraryInit =
        init(
            ( options.isItNecessaryToParseLinksFile() ? 0 : NO_LINKS ) |
            ( options.isItNecessaryToParseTagFile() ? 0 : NO_TAGS ) |
            ( options.isVerbose() ? VERBOSE : 0 ) |
            ( options.isItNecessaryToParseDetailedFile() ? DETAILED : 0 ) |
            ( options.isItNecessaryToParseListFile() ? 0 : NO_LISTS )
        );

    if( libraryInit == EXIT_SUCCESS )
    {
#       ifdef HAVE_CURL_CURL_H
        if( options.downloadRequested() )
        {
            // download files if they don't exist
            downloadIf( !options.isItNecessaryToParseDetailedFile(), SENTENCES_URL, SENTENCES_FILENAME );
            downloadIf( options.isItNecessaryToParseDetailedFile(), DETAILED_URL, DETAILED_FILENAME );
            downloadIf( options.isItNecessaryToParseLinksFile(), LINKS_URL, LINKS_FILENAME );
            downloadIf( options.isItNecessaryToParseTagFile(), TAG_URL, TAG_FILENAME );
            downloadIf( options.isItNecessaryToParseListFile(), LIST_URL, LIST_FILENAME );
        }
#       endif

        const int libraryParsing =
            parse( allSentences, allLinks, allTags, allLists,
                   options.isItNecessaryToParseDetailedFile() ?
                   csvPath + '/' + DETAILED_FILENAME     :
                   csvPath + '/' + SENTENCES_FILENAME,
                   csvPath + '/' + LINKS_FILENAME,
                   csvPath + '/' + TAG_FILENAME,
                   csvPath + '/' + LIST_FILENAME );

        skipFiltering |= ( libraryParsing != EXIT_SUCCESS );
    }
    else
        skipFiltering = true;

    if( !skipFiltering )
    {
        options.treatTranslations( allLinks, allFilters );

        // if a list has beeg given, check if the list exists
        const std::string & lowerCaseListName = toLower( options.getListName() );

        if( lowerCaseListName.size() && allLists.doesListExist( lowerCaseListName ) == false )
        {
            qlog::error << "list \"" << lowerCaseListName << "\" does not exist in lists.csv" << qlog::color() << '\n';
            skipFiltering = true;
        }
    }

    ///////////////////////////
    //  filtering sentences  //
    ///////////////////////////
    if( !skipFiltering )
    {
        std::vector<const sentence *> filteredSentences;

        // go through every sentence and see if it matches the filter
        auto endFilter = allFilters.end();
        unsigned printedLineNumber = 0;

        bool shouldDisplay = true, keepSentence = true;

        for( const sentence & sentence : allSentences )
        {
            if( sentence.getId() == sentence::INVALID_ID )
                continue;

            keepSentence = true;

            for( auto filter = allFilters.begin(); shouldDisplay && filter != endFilter; ++filter )
            {
                keepSentence &= ( *filter )->parse( sentence );
            }

            // display the sentence if it is seleted by all the filters
            if( keepSentence )
            {
                filteredSentences.push_back( &sentence );
            }
        }

        /////////////////////////////////////
        //  processing filtered sentences  //
        /////////////////////////////////////
        for( const sentence * sentence : filteredSentences )
        {
            assert(nullptr != sentence);

            if( sentence->getId() == sentence::INVALID_ID )
                continue;
            shouldDisplay = true;

            for( auto filter = allFilters.begin(); shouldDisplay && filter != endFilter; ++filter )
            {
                shouldDisplay &= ( *filter )->postProcess( *sentence );
            }

            if( shouldDisplay )
            {
                displaySentence( options, allSentences, allLinks, *sentence, ++printedLineNumber );
            }
        }
    }

    #ifdef NAMESPACE
    NAMESPACE ::
    #endif
    terminate();

    return EXIT_SUCCESS;
}

// -------------------------------------------------------------------------- //

void startLog( bool verbose )
{
    using namespace qlog;

    qlog::set_loglevel( verbose ? qlog::loglevel::trace : qlog::loglevel::warning );

    qlog::info.reset_decoration();
    qlog::warning.reset_decoration();
    qlog::error.reset_decoration();

    // prepend markers
    qlog::info.prepend() << "[..] ";
    qlog::warning.prepend() << "[" << color( green ) << "ww" << color() << "] ";
    qlog::error.prepend() << "[" << color( red, true ) << "EE" << color() << "] " << color( white, true );

    // append just resets color
    qlog::error.append() << color();

    qlog::set_output( std::cerr );
    qlog::cout.set_output( std::cout );
}

// -------------------------------------------------------------------------- //

#ifdef HAVE_CURL_CURL_H
/**@brief Download an url to a file if a condition is met
 * @param[in] _condition If this parameter evals to tru, then the download is started.
 * @param[in] _url The url to download the file from (This must
 *            be a valid URL).
 * @param[in] _filename The name of the file to download the data to. If the file exists, then
 *            the download is skipped
 * @return true if the download succeeds and the file is written, false otherwise
 */
bool downloadIf( bool _condition, std::string _url, std::string _filename )
{
    bool ret = false;

    if( _condition )
    {
        std::fstream destinationFile( _filename, std::ios::out | std::ios::in );  // will not create file
        if( destinationFile.is_open() )
        {
            qlog::info << "Found " << _filename.c_str() << ", not downloading it\n";
        }
        else
        {
            destinationFile.clear();
            destinationFile.open( _filename.c_str(), std::ios::out );
            qlog::info << "Downloading " << _filename.c_str() << "...\n";

            try
            {
                hdl::downloader download( _url );

                download.followRedirect( true );
                download.throwExceptionOnDownloadFailure( false );

                ret = download.stream( destinationFile );

                qlog::info( ret ) << "Finished downloading " << _filename.c_str() << '\n';
                qlog::warning( !ret ) << "Failed to download " << _filename.c_str() << '\n';
            }
            catch( const std::bad_alloc & )
            {
                qlog::error << "Out of memory.\n";
            }
            catch( const hdl::curl_exception & )
            {
                qlog::error << "Curl failed to initialize.\n";
            }
        }
    }

    return ret;
}
#endif


// -------------------------------------------------------------------------- //

void displaySentence( userOptions & _options, dataset & _allSentences, linkset & _allLinks, const sentence & _sentence, unsigned _lineNumber )
{
    using namespace qlog;
    const std::string & separator = _options.getSeparator();
    std::string translationLanguage = _options.getFirstTranslationLanguage();

    // option -n
    if( _options.displayLineNumbers() )
        qlog::cout << _lineNumber << separator;

    // option -i
    if( _options.displayIds() )
        qlog::cout << ( _options.isColored() ? color( yellow ) : color() ) << _sentence.getId() << color() << separator;

    // option --display-lang
    if( _options.displayLanguages() )
        qlog::cout << ( _options.isColored() ? color( red ) : color() ) << _sentence.lang() << color() << separator;

    // display the sentence
    qlog::cout << _sentence.str();

    // display a translation if it was requested
    if( _options.displayFirstTranslation() )
    {
        // find a suitable translation
        const sentence::id firstTranslationId =
            getFirstSentenceTranslation(
                _allSentences,
                _allLinks,
                _sentence.getId(),
                translationLanguage
            );

        if( firstTranslationId != sentence::INVALID_ID
                && _allSentences[firstTranslationId] )
        {
            // display the translation
            qlog::cout << separator << ( _options.isColored() ? color( cyan ) : color() ) << _allSentences[firstTranslationId]->str() << color();
        }
    }

    qlog::cout << '\n';
}
