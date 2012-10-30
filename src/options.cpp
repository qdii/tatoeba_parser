#include "prec.h"
#include "options.h"
#include "filter_id.h"
#include "filter_regex.h"
#include "filter_translation_regex.h"
#include "filter_link.h"
#include "filter_lang.h"
#include "filter_tag.h"
#include "filter_translatable_in_language.h"
#include <tatoparser/dataset.h>
#include <tatoparser/tagset.h>
#include <tatoparser/linkset.h>
#include <tatoparser/sentence.h>
#include <boost/program_options/cmdline.hpp>
#include <boost/program_options/parsers.hpp>
#include <fstream>

#ifdef HAVE_SYS_RESOURCE_H
#   include <sys/resource.h>
#endif

NAMESPACE_START

namespace po = boost::program_options;

userOptions::userOptions()
    :m_desc( "" )
    ,m_visibleOptions()
    ,m_vm()
    ,m_separator( "\t" )
    ,m_configFileDescriptions()
    ,m_configFileCsvPath()
    ,m_configFileAcceptedLanguages()
{
    // ----- general options
    po::options_description generalOptions( "" );
    generalOptions.add_options()
        ( "help,h", "Produce help message." )
        ( "verbose,v", "Displays more info." )
        ( "csv-path", po::value<std::string>(), "Sets the path where sentences.csv, links.csv and tags.csv will be found." )
        ( "config-path", po::value<std::string>(), "Sets the path of the config file. ~/.tatoparser will be used by default." )
        ( "version", "Displays the current version of the program." )
    ;
    m_desc.add( generalOptions );
    m_visibleOptions.add( generalOptions );

    // ----- filters
    po::options_description filteringOptions( "Use those options to select the sentences to display" );
    filteringOptions.add_options()
        ( "has-id", po::value<sentence::id>(), "Filters out sentences which ids are different to this one." )
        ( "regex,r", po::value<std::vector<std::string> >()->composing(), "A regular expression that the sentence should match entirely." )
        ( "is-linked-to", po::value<sentence::id>(), "Filters only sentences that are a translation of the given id." )
        ( "is-translatable-in", po::value<std::string>(), "Keep the sentence if it has a translation in a given language." )
        ( "language,l", po::value<std::vector<std::string>>(), "Filter out sentences which languages is different from the ones given. Accept multiple values." )
        ( "has-tag,g", po::value<std::string>(), "Checks if the sentence has a given tag." )
        ( "translation-regex,p", po::value<std::vector<std::string> >()->composing(),
          "Filters only sentences which translations match this regex. If many "
          "regular expressions are provided, a sentence will be kept if any of its "
          "translations matches them all." )
    ;
    m_desc.add( filteringOptions );
    m_visibleOptions.add( filteringOptions );

    // ----- display
    po::options_description displayOptions( "Use those options to change the way sentences are displayed" );
    displayOptions.add_options()
        ( "display-line-numbers,n", "Display the indexes of the lines." )
        ( "display-ids,i", "Displays the sentence ids." )
        ( "display-first-translation", po::value<std::string>(), "Display the first translation of the sentence in a given language." )
        ( "separator", po::value<std::string>( &m_separator ), "Sets the separator character ('\\t' by default)." )
    ;
    m_desc.add( displayOptions );
    m_visibleOptions.add( displayOptions );


    // ----- debug
    po::options_description debugOptions( "Debug settings" );
    debugOptions.add_options()
        ( "just-parse", "Do not actually do anything but parsing. Useful for debug." )
#ifdef HAVE_SYS_RESOURCE_H
        ( "limit-mem", po::value<rlim_t>(), "limit the available virtual space." )
#endif
    ;
    m_desc.add( debugOptions );
#ifndef NDEBUG
    m_visibleOptions.add( debugOptions );
#endif
    declareConfigFileValidOptions();
}

// -------------------------------------------------------------------------- //

/**@brief let boost::program_options treat the user command line arguments */
void userOptions::treatCommandLine( int argc, char * argv[] )
{
    po::store( po::parse_command_line( argc, argv, m_desc ), m_vm );
    po::notify( m_vm );
}

// -------------------------------------------------------------------------- //

/**@brief Populate the passed list of filters with certain filters */
void userOptions::getFilters( dataset & _dataset, linkset & _linkset, tagset & _tagset, FilterVector & allFilters_ )
{
    using std::shared_ptr;
    using std::vector;
    using std::string;

    qlog::warning( allFilters_.size() ) << "allFilters.size() > 0\n";

    if( m_vm.count( "has-id" ) )
    {
        allFilters_.push_back(
            shared_ptr<filter>(
                new filterId( m_vm["has-id"].as<sentence::id>() )
            )
        );
    }

    // The various filters will be applied in order. The language filter is
    // very light so we want it first to discard as many sentences as possible
    if( m_vm.count( "language" ) )
    {
        allFilters_.push_back(
            shared_ptr<filter>(
                new filterLang( m_vm["language"].as<vector<string>>() )
            )
        );
    }
    else if ( m_configFileAcceptedLanguages.size() )
    {
        allFilters_.push_back(
            shared_ptr<filter>(
                new filterLang( m_configFileAcceptedLanguages )
            )
        );
    }

    if( m_vm.count( "is-linked-to" ) )
    {
        allFilters_.push_back(
            shared_ptr<filter>(
                new filterLink( _linkset, m_vm["is-linked-to"].as<sentence::id>() )
            )
        );
    }

    if( m_vm.count( "has-tag" ) )
    {
        allFilters_.push_back(

            shared_ptr<filter>(
                new filterTag( _tagset, m_vm["has-tag"].as<string>() )

            )
        );
    }

    if( m_vm.count( "is-translatable-in" ) )
    {
        allFilters_.push_back(
            shared_ptr<filter>(
                new filterTranslatableInLanguage(
                    _dataset, _linkset,
                    m_vm["is-translatable-in"].as<string>()
                )
            )
        );
    }

    // regular expression filters are added last as each of those filters is
    // relatively heavy.
    if( m_vm.count( "regex" ) )
    {
        // for each regex, we create a filter
        const vector< string > & allRegex = m_vm["regex"].as<vector<string>>();

        for( auto regex : allRegex )
        {
            shared_ptr<filter> newFilter =
                shared_ptr<filter>( new filterRegex( regex ) );
            allFilters_.push_back( newFilter );
        }
    }

    if( m_vm.count( "translation-regex" ) )
    {
        allFilters_.push_back(
            shared_ptr<filter>(
                new filterTranslationRegex(
                    _dataset, _linkset,
                    m_vm["translation-regex"].as<vector<string>>() )
            )
        );
    }

#ifdef HAVE_SYS_RESOURCE_H

    if( m_vm.count( "limit-mem" ) )
    {
        struct rlimit lim;
        getrlimit( RLIMIT_AS, &lim );
        qlog::info << "virtual space soft limit was " << lim.rlim_cur << '\n';
        lim.rlim_cur =  m_vm["limit-mem"].as<rlim_t>();
        const int res = setrlimit( RLIMIT_AS, &lim );
        qlog::info( res == 0 ) << "virtual space soft limit has been set to " << lim.rlim_cur << '\n';
        qlog::warning( res != 0 ) << "setrlimit call failed\n";
    }

#endif

    qlog::info << "registered " << allFilters_.size() << " filters\n";
}

// -------------------------------------------------------------------------- //

/**@brief Outputs the version (defined in config.h) */
void userOptions::printVersion()
{
    std::cout   << PACKAGE_STRING << "\n"
                << "Please report bugs to " << PACKAGE_BUGREPORT << "\n";
}

// -------------------------------------------------------------------------- //

void userOptions::treatConfigFile()
{
    const std::string configFilePath =
        std::string( m_vm.count( "config-path" ) ?
                     m_vm["config-path"].as<std::string>() : getenv( "HOME" ) ) + "/.tatoparser";

    qlog::info << "parsing config file in " << configFilePath << '\n';

    try
    {
        po::basic_parsed_options<char> configFileOptions =
            po::parse_config_file<char>( configFilePath.c_str(), m_configFileDescriptions );

        // treat options
        for( po::basic_option<char> opt : configFileOptions.options )
        {
            if( opt.string_key == "csv-path" )
            {
                assert( opt.value.size() );
                qlog::info << "csv-path: " << opt.value[opt.value.size()-1] << '\n';
                m_configFileCsvPath = opt.value[opt.value.size()-1];
            }
            else if ( opt.string_key == "lang" )
            {
                for( size_t languageIndex = 0; languageIndex < opt.value.size(); ++languageIndex )
                    m_configFileAcceptedLanguages.push_back( opt.value[languageIndex] );
            }
            else
            {
                qlog::warning << "\tunknown token:" << opt.original_tokens[0] << '\n';
            }
        }


    }
    catch( const po::error & exc )
    {
        qlog::warning << "Failed to parse config file (" << exc.what() << ")\n";
    }
}

// -------------------------------------------------------------------------- //

void userOptions::declareConfigFileValidOptions()
{
    m_configFileDescriptions.add_options()
        ( "csv-path", po::value<std::string>(), "Sets the path where sentences.csv, links.csv and tags.csv will be found." )
        ( "lang", po::value<std::vector<std::string>>(), "Only keep a sentence that matches this language. Multiple entries allowed." )
    ;
}

// -------------------------------------------------------------------------- //

NAMESPACE_END
