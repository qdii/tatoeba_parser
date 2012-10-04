#include "prec.h"
#include "options.h"
#include "sentence.h"
#include "filter_regex.h"
#include "filter_translation_regex.h"
#include "filter_link.h"
#include "filter_lang.h"
#include "filter_tag.h"
#include "filter_translatable_in_language.h"
#include "dataset.h"
#include "tagset.h"
#include "linkset.h"
#include <boost/program_options/cmdline.hpp>
#include <boost/program_options/parsers.hpp>

#ifdef TATO_MEM_DEBUG
#   include <sys/resource.h>
#endif

NAMESPACE_START

namespace po = boost::program_options;

userOptions::userOptions()
    :m_desc( "" )
    ,m_vm()
{
    m_desc.add_options()
    ( "help,h", "Produce help message." )
    //( "compulsory,c", po::value<std::string>(), "The characters that should appear in the sentence." )
    //( "optional,o", po::value<std::string>(), "The characters that may compose the sentence." )
    ( "display-line-numbers,n", "Display the indexes of the lines." )
    ( "language,l", po::value<std::string>(), "Restrict the languages to a given one." )
    ( "display-ids,i", "Displays the sentence ids." )
    //( "separator,s", po::value<char>(), "Changes the separator characters, default is '\\t'" )
    ( "regex,r", po::value<std::vector<std::string> >()->composing(), "A regular expression that the sentence should match entirely." )
    ( "is-linked-to", po::value<sentence::id>(), "Filters only sentences that are a translation of the given id." )
    //( "depedent-regex,d",  po::value<std::vector<std::string> >()->composing(), "A set of regular expressions which have to be all matched." )
    ( "is-translatable-in", po::value<std::string>(), "Keep the sentence if it has a translation in a given language." )
    //( "translation-contains-regex,j", po::value<std::string>(), "A regex that one of the translation of the sentence should match." )
    ( "verbose,v", "Displays more info." )
    ( "has-tag,g", po::value<std::string>(), "Checks if the sentence has a given tag." )
    //( "translates", po::value<sentence::id>(), "Checks if the sentence is a translation of the given sentence id" )
    ( "translation-regex,p", po::value<std::vector<std::string> >()->composing(),
      "Filters only sentences which translations match this regex. If many "
      "regular expressions are provided, a sentence will be kept if any of its "
      "translations matches them all." )
    ( "csv-path", po::value<std::string>(), "Sets the path where sentences.csv, links.csv and tags.csv will be found." )
    ( "version", "Displays the current version of the program." )
    ( "just-parse", "Do not actually do anything but parsing. Useful for debug.")

#ifdef TATO_MEM_DEBUG
    ( "limit-mem", po::value<rlim_t>(), "limit the available virtual space.")
#endif
    ;
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
    qlog::warning( allFilters_.size() ) << "allFilters.size() > 0\n";

    // The various filters will be applied in order. The language filter is
    // very light so we want it first to discard as many sentences as possible
    if( m_vm.count( "language" ) )
    {
        allFilters_.push_back(
            std::shared_ptr<filter>(
                new filterLang( m_vm["language"].as<std::string>() )
            )
        );
    }

    if( m_vm.count( "is-linked-to" ) )
    {
        allFilters_.push_back(
            std::shared_ptr<filter>(
                new filterLink( _linkset, m_vm["is-linked-to"].as<sentence::id>() )
            )
        );
    }

    if( m_vm.count( "has-tag" ) )
    {
        allFilters_.push_back(

            std::shared_ptr<filter>(
                new filterTag( _tagset, m_vm["has-tag"].as<std::string>() )

            )
        );
    }

    if( m_vm.count( "is-translatable-in" ) )
    {
        allFilters_.push_back(
            std::shared_ptr<filter>(
                new filterTranslatableInLanguage(
                    _dataset, _linkset,
                    m_vm["is-translatable-in"].as<std::string>()
                )
            )
        );
    }

    // regular expression filters are added last as each of those filters is
    // relatively heavy.
    if( m_vm.count( "regex" ) )
    {
        // for each regex, we create a filter
        const std::vector< std::string > & allRegex = m_vm["regex"].as<std::vector<std::string>>();

        for( auto regex : allRegex )
        {
            std::shared_ptr<filter> newFilter =
                std::shared_ptr<filter>( new filterRegex( regex ) );
            allFilters_.push_back( newFilter );
        }
    }

    if( m_vm.count( "translation-regex" ) )
    {
        allFilters_.push_back(
            std::shared_ptr<filter>(
                new filterTranslationRegex(
                    _dataset, _linkset,
                    m_vm["translation-regex"].as<std::vector<std::string>>() )
            )
        );
    }
#ifdef TATO_MEM_DEBUG
    if ( m_vm.count( "limit-mem" ))
    {
        struct rlimit lim;
        getrlimit( RLIMIT_AS, &lim );
        qlog::info << "virtual space soft limit was " << lim.rlim_cur << '\n';
        lim.rlim_cur =  m_vm["limit-mem"].as<rlim_t>();
        const int res = setrlimit( RLIMIT_AS, &lim );
        qlog::info(res == 0) << "virtual space soft limit has been set to " << lim.rlim_cur << '\n';
        qlog::warning(res != 0) << "setrlimit call failed\n";
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

NAMESPACE_END
