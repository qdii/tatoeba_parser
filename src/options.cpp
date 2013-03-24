#include "prec.h"
#include "options.h"
#include "filter_id.h"
#include "filter_idlist.h"
#include "filter_regex.h"
#include "filter_translation_regex.h"
#include "filter_link.h"
#include "filter_list.h"
#include "filter_lang.h"
#include "filter_tag.h"
#include "filter_fuzzy.h"
#include "filter_translatable_in_language.h"
#include "filter_user.h"
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

static
void validate( boost::any & _v,
               const std::vector<std::string> & _values,
               fuzzyFilterOption * _options, int )
{
    // Make sure no previous assignment to 'a' was made.
    po::validators::check_first_occurrence( _v );

    fuzzyFilterOption temporaryOptions;

    temporaryOptions.numberOfMatch = boost::lexical_cast<unsigned int>( _values.at( 0 ) );
    temporaryOptions.expression = _values.at( 1 );

    _v = temporaryOptions;
}

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
#ifdef HAVE_CURL_CURL_H
        ( "download", "Download necessary csv files if not found." )
#endif
        ( "version", "Displays the current version of the program." )
    ;
    m_desc.add( generalOptions );
    m_visibleOptions.add( generalOptions );

    // ----- filters
    po::options_description filteringOptions( "Use those options to select the sentences to display" );
    filteringOptions.add_options()
        ( "has-id", po::value<sentence::id>(), "Filters out sentences which ids are different to this one." )
        ( "regex,r", po::value<std::vector<std::string> >()->composing(), "One or more regular expressions that the sentence should match entirely." )
        ( "regex-nocs", po::value<std::vector<std::string> >()->composing(), "One or more regular expressions that the sentence should match entirely regardless of the case." )
        ( "is-linked-to", po::value<sentence::id>(), "Filters only sentences that are a translation of the given id." )
        ( "is-translatable-in", po::value<std::string>(), "Keep the sentence if it has a translation in a given language." )
        ( "language,l", po::value<std::vector<std::string>>(), "Filter out sentences which languages is different from the ones given. Accept multiple values." )
        ( "has-tag,g", po::value<std::string>(), "Checks if the sentence has a given tag." )
        ( "translation-regex,p", po::value<std::vector<std::string> >()->composing(),
          "Filters only sentences which translations match this regex. If many "
          "regular expressions are provided, a sentence will be kept if any of its "
          "translations matches them all." )
        ( "user,u", po::value<std::string>(), "Keep the sentences which belong to this user only." )
        ( "in-list", po::value<std::string>(), "Keep the sentences which belong to a given list." )
        ( "orphan", "Keep sentences that belong to no-one." )
        ( "translates,t", po::value<sentence::id>(), "Keep the indirect and direct translations of a given sentence." )
        ( "fuzzy,f", po::value<fuzzyFilterOption>()->multitoken(), "Looks for the N sentences that look like the given expression." )
    ;
    m_desc.add( filteringOptions );
    m_visibleOptions.add( filteringOptions );

    // ----- display
    po::options_description displayOptions( "Use those options to change the way sentences are displayed" );
    displayOptions.add_options()
        ( "display-line-numbers,n", "Display the indexes of the lines." )
        ( "display-ids,i", "Displays the sentence ids." )
        ( "display-first-translation", po::value<std::string>(), "Display the first translation of the sentence in a given language." )
        ( "display-lang", "Displays the language of the sentence." )
        ( "separator", po::value<std::string>( &m_separator ), "Sets the separator character ('\\t' by default)." )
        ( "color,c", "Add color to the output." )
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
    po::store( po::command_line_parser( argc, argv ).options(m_desc).run(), m_vm );
    po::notify( m_vm );
}

// -------------------------------------------------------------------------- //

template< typename FILTER_TYPE, typename ... ARGS >
static
bool addNewFilterToListGeneric( boost::program_options::variables_map & _vm, const std::string & _option, FilterVector & allFilters_, bool _condition, ARGS && ... args )
{
    if( _condition )
    {
        qlog::info << "Adding filter for option: " << qlog::color( qlog::blue ) << _option << qlog::color() << '\n';
        allFilters_.push_back(
            std::shared_ptr<filter>(
                new FILTER_TYPE( args... )
            )
        );
    }

    return _condition;
}

// -------------------------------------------------------------------------- //

template< typename OPTION_TYPE, typename FILTER_TYPE, typename ... ARGS >
static inline
bool addNewFilterToList( boost::program_options::variables_map & _vm, const std::string & _option, FilterVector & allFilters_, ARGS && ... args )
{
    const bool optionWasSelected = _vm.count( _option.c_str() ) > 0;
    return optionWasSelected &&
           addNewFilterToListGeneric<FILTER_TYPE>( _vm, _option, allFilters_, optionWasSelected, _vm[_option.c_str()].as<OPTION_TYPE>(), args... );
}

// -------------------------------------------------------------------------- //

/**@brief Populate the passed list of filters with certain filters */
void userOptions::getFilters( dataset & _dataset, linkset & _linkset, tagset & _tagset, listset & _listset, FilterVector & allFilters_ )
{
    using std::shared_ptr;
    using std::vector;
    using std::string;

    qlog::warning( allFilters_.size() > 0 ) << "allFilters.size() > 0\n";

    addNewFilterToList<sentence::id, filterId>( m_vm, "has-id", allFilters_ );
    addNewFilterToList<std::string, filterUser>( m_vm, "user", allFilters_ );
    addNewFilterToListGeneric< filterUser >( m_vm, "orphan", allFilters_, m_vm.count( "orphan" ) > 0, "", true );

    // The various filters will be applied in order. The language filter is
    // very light so we want it first to discard as many sentences as possible

    if( addNewFilterToList<vector<string>, filterLang>( m_vm, "language", allFilters_ ) == false )
    {
        addNewFilterToListGeneric<filterLang>( m_vm, "language", allFilters_, m_configFileAcceptedLanguages.size(), m_configFileAcceptedLanguages );
    }

    addNewFilterToList<sentence::id, filterLink>( m_vm, "is-linked-to", allFilters_, _linkset );
    addNewFilterToList<std::string, filterTranslatableInLanguage>( m_vm, "is-translatable-in", allFilters_, _dataset, _linkset );

    // regular expression filters are added last as each of those filters is
    // relatively heavy.
    if( m_vm.count( "regex" ) )
    {
        // for each regex, we create a filter
        const vector< string > & allRegex = m_vm["regex"].as<vector<string>>();
        qlog::info << "Adding filter for options: " << qlog::color( qlog::blue ) << "--regex" << qlog::color() << '\n';
        for( auto regex : allRegex )
        {
            shared_ptr<filter> newFilter =
                shared_ptr<filter>( new filterRegex( regex ) );
            allFilters_.push_back( newFilter );
        }
    }

    if( m_vm.count( "regex-nocs" ) )
    {
        qlog::info << "Adding filter for options: " << qlog::color( qlog::blue ) << "--regex-nocs" << qlog::color() << '\n';
        // for each regex, we create a filter
        const vector< string > & allRegex = m_vm["regex-nocs"].as<vector<string>>();

        for( auto regex : allRegex )
        {
            shared_ptr<filter> newFilter =
                shared_ptr<filter>( new filterRegex( regex, false ) );
            allFilters_.push_back( newFilter );
        }
    }

    addNewFilterToList<vector<string>, filterTranslationRegex>( m_vm, "translation-regex", allFilters_, _dataset, _linkset );
    addNewFilterToList<std::string, filterList>( m_vm, "in-list", allFilters_, _listset );
    addNewFilterToList<std::string, filterTag>( m_vm, "has-tag", allFilters_, _tagset );

    if( m_vm.count( "fuzzy" ) > 0 )
        addNewFilterToListGeneric<filterFuzzy>( m_vm, "fuzzy", allFilters_, true, m_vm["fuzzy"].as<fuzzyFilterOption>() );

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

static
std::string getEnvironmentVariable( const char * _environmentVariable )
{
    std::string ret = "";
#ifdef WIN32
    char * pValue;
    size_t len;

    errno_t err = _dupenv_s( &pValue, &len, _environmentVariable );
    if( err )
        return ret;

    ret.assign( pValue, pValue + len );
    free( pValue );

#else
    ret.assign( getenv( _environmentVariable ) );
#endif

    return ret;
}

void userOptions::treatConfigFile()
{
    const std::string configFilePath =
        std::string( m_vm.count( "config-path" ) ?
                     m_vm["config-path"].as<std::string>() : getEnvironmentVariable( "HOME" ) ) + "/.tatoparser";

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
            else if( opt.string_key == "lang" )
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

void userOptions::addTranslationFilters( sentence::id _id, const linkset & _allLinks, std::vector<sentence::id> & allTranslations_ )
{
    qlog::debug << "adding translation: " << qlog::color( qlog::yellow ) << _id << qlog::color() << '\n';
    allTranslations_.push_back( _id );
    auto iterators = _allLinks.getLinksOfSafe( _id );
    for( linkset::const_iterator current = iterators.first; current != iterators.second; ++current )
    {
        if( std::find( allTranslations_.begin(), allTranslations_.end(), *current ) == allTranslations_.end() )
            addTranslationFilters( *current, _allLinks, allTranslations_ );
    }
}

// -------------------------------------------------------------------------- //

void userOptions::treatTranslations( const linkset & _allLinks, FilterVector & allFilters_ )
{
    if( m_vm.count( "translates" ) > 0 )
    {
        sentence::id sentenceToTranslate = m_vm["translates"].as<sentence::id>();
        std::vector<sentence::id> allTranslations;
        addTranslationFilters( sentenceToTranslate, _allLinks, allTranslations );

        assert( allTranslations.empty() == false );

        std::shared_ptr<filter> translationIdFilter( new filterIdList( std::move( allTranslations ) ) );
        allFilters_.push_back( translationIdFilter );
    }
}

NAMESPACE_END
