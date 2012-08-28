#include "prec.h"
#include "parser.h"
#include "selecter.h"
#include "filter_regex.h"
#include "filter_language.h"
#include "filter_translation.h"
#include "filter_tag.h"
#include "filter_id.h"
#include "filter_dependent.h"
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/cmdline.hpp>
#include <boost/program_options/parsers.hpp>
#include <iostream>
#include <vector>

namespace po = boost::program_options;

NAMESPACE_START

cout_warning * gs_coutWarning;

NAMESPACE_END
USING_NAMESPACE

//------------------------------------------------------------

template<typename T>
T * createFilter( bool _condition, std::vector<Filter *> & _filtersToDelete, selecter * _selecter )
{
    T * filter = nullptr;

    if( _condition )
    {
        filter = new T;

        if( filter )
        {
            _filtersToDelete.push_back( filter );

            if( _selecter )
                _selecter->addFilter( *filter );
        }
    }

    return filter;
}

//------------------------------------------------------------

int main( int argc, char * argv[] )
{
    // initializing outputs
    cout_warning warning_output;
    gs_coutWarning = & warning_output;

    // command line parameters
    po::options_description desc( "Allowed options" );
    desc.add_options()
        ( "help,h", "produce help message" )
        ( "compulsory,c", po::value<std::string>(), "The characters that should appear in the sentence." )
        ( "optional,o", po::value<std::string>(), "The characters that may compose the sentence." )
        ( "line-numbers,n", "Display the indexes of the lines." )
        ( "language,l", po::value<std::string>(), "Restrict the languages to a given one." )
        ( "display-ids,i", "Displays the sentence ids." )
        ( "separator,s", po::value<char>(), "Changes the separator characters, default is '\\t'" )
        ( "regex,r", po::value<std::vector<std::string> >()->composing(), "A regular expression that the sentence should match entirely." )
        ( "depedent-regex,d",  po::value<std::vector<std::string> >()->composing(), "A set of regular expressions which have to be all matched." )
        ( "translatable-in,t", po::value<std::string>(), "A language that the sentence can be translated into." )
        ( "translation-contains-regex,j", po::value<std::string>(), "A regex that one of the translation of the sentence should match." )
        ( "verbose,v", "Displays warnings" )
        ( "has-tag,g", po::value<sentence::tag>(), "Checks if the sentence has a given tag" )
        ( "translates", po::value<sentence::id>(), "Checks if the sentence is a translation of the given sentence id" )
        ( "translation-matches-dependent-regex,p", po::value<std::vector<std::string> >()->composing(), "A set of regular expressions that one of the translation of the sentence has to match all." )
    ;

    po::variables_map vm;
    po::store( po::parse_command_line( argc, argv, desc ), vm );
    po::notify( vm );

    if( vm.count( "help" ) )
    {
        std::cout << desc << "\n";
        return 1;
    }

    // do not output the error messages if the user does not want that
    if( !vm.count( "verbose" ) )
        VERIFY_EQ( gs_coutWarning->mute(), SUCCESS );

    // parse the files
    const bool areLinksNecessary =
        vm.count( "translatable-in" )
        ||  vm.count( "translation-contains-regex" )
        ||  vm.count( "translates" )
        ||  vm.count( "translation-matches-dependent-regex" );
    const std::string sentencesFile( "sentences.csv" );
    const std::string linksFile( areLinksNecessary ? "links.csv" : "" );
    const std::string tagsFile( vm.count( "has-tag" ) ? "tags.csv" : "" );
    parser tatoeba_parser( sentencesFile, linksFile, tagsFile );

    dataset allSentences; // the structure that will contain all the sentences
    VERIFY_EQ( tatoeba_parser.setOutput( allSentences ),    SUCCESS );

    // ###### SET UP THE FILTERS #####
    selecter sel;
    std::vector< Filter * > filtersToDelete; // we keep the filter pointers that need to be deleted here

    // LANGUAGE FILTER
    FilterLanguage * const languageFilter =
        createFilter<FilterLanguage>( vm.count( "language" ), filtersToDelete, &sel );

    if( languageFilter )
        languageFilter->setCountryCode( vm["language"].as<std::string>() );

    // TAG FILTER
    FilterTag * const languageTag =
        createFilter<FilterTag>( vm.count( "has-tag" ), filtersToDelete, &sel );

    if( languageTag )
        languageTag->setTag( vm["has-tag"].as<sentence::tag>() );

    // OPTIONAL CHARACTERS
    FilterRegex * const optionalRegexFilter =
        createFilter<FilterRegex>( vm.count( "optional" ), filtersToDelete, &sel );

    if( optionalRegexFilter )
        optionalRegexFilter->setRegex( std::string( "[" + vm["optional"].as<std::string>() + "]*" ) );


    // MANDATORY CHARACTERS
    FilterRegex * const compulsoryRegexFilter =
        createFilter<FilterRegex>( vm.count( "compulsory" ), filtersToDelete, &sel );

    if( compulsoryRegexFilter )
        compulsoryRegexFilter->setRegex( std::string( ".*[" + vm["compulsory"].as<std::string>() + "]+.*" ) );


    // USER DEFINED REGEX
    if( vm.count( "regex" ) )
    {
        const std::vector<std::string> && userRegexList = std::move( vm["regex"].as<std::vector<std::string> >() );

        for( auto regularExpression : userRegexList )
        {
            FilterRegex * const userRegexFilter =
                createFilter<FilterRegex>( true, filtersToDelete, &sel );

            if( userRegexFilter )
                userRegexFilter->setRegex( regularExpression );
        }
    }

    // DEPENDENT REGEX
    FilterDependent * const dependentRegexFilter =
        createFilter<FilterDependent>( vm.count( "depedent-regex" ), filtersToDelete, &sel );

    if( dependentRegexFilter )
    {
        const std::vector<std::string> && userRegexList = std::move( vm["depedent-regex"].as<std::vector<std::string> >() );

        for( auto regularExpression : userRegexList )
        {
            FilterRegex * const userRegexFilter =
                createFilter<FilterRegex>( true, filtersToDelete, nullptr );

            if( userRegexFilter )
            {
                userRegexFilter->setRegex( regularExpression );
                dependentRegexFilter->addFilter( *userRegexFilter );
            }
        }
    }

    // FILTERS ON TRANSLATIONS
    FilterTranslation * const translationFilter =
        createFilter<FilterTranslation>( areLinksNecessary, filtersToDelete, &sel );

    if( translationFilter )
    {
        // language
        FilterLanguage * const langFilter =
            createFilter<FilterLanguage>( vm.count( "translatable-in" ), filtersToDelete, nullptr );

        if( langFilter )
        {
            langFilter->setCountryCode( vm["translatable-in"].as<std::string>() );
            translationFilter->addFilter( *langFilter );
        }

        // regex
        FilterRegex * const regexFilter =
            createFilter<FilterRegex>( vm.count( "translation-contains-regex" ), filtersToDelete, nullptr );

        if( regexFilter )
        {
            regexFilter->setRegex( vm["translation-contains-regex"].as<std::string>() );
            translationFilter->addFilter( *regexFilter );
        }

        // id
        FilterId * const idFilter =
            createFilter<FilterId>( vm.count( "translates" ), filtersToDelete, nullptr );

        if( idFilter )
        {
            idFilter->setId( vm["translates"].as<sentence::id>() );
            translationFilter->addFilter( *idFilter );
        }

        FilterDependent * const dependentFilter =
            createFilter<FilterDependent>( vm.count( "translation-matches-dependent-regex" ), filtersToDelete, nullptr );
        if (dependentFilter)
        {
            const std::vector<std::string> && userRegexList = 
                std::move( vm["translation-matches-dependent-regex"].as<std::vector<std::string> >() );
            for( auto regularExpression : userRegexList )
            {
                FilterRegex * const regexDepFilter = 
                    createFilter<FilterRegex>( userRegexList.size() > 0, filtersToDelete, nullptr );
                
                if (regexDepFilter)
                {
                    regexDepFilter->setRegex( regularExpression );
                    dependentFilter->addFilter( *regexDepFilter );
                }
            }

            VERIFY_EQ( translationFilter->addFilter( *dependentFilter ), SUCCESS );
        }
    }

    // START PARSING
    const int parseSuccess = tatoeba_parser.start();

    if( parseSuccess == CANT_OPEN_SENTENCES_CSV )
        ERR << "Unable to open \"sentences.csv\"\n";
    else if( tagsFile != "" && parseSuccess == CANT_OPEN_TAGS_CSV )
        ERR << "Unable to open \"tags.csv\"\n";
    else
    {
        WARN << "OK, parsed " << allSentences.size() << " sentences\n";

        if( parseSuccess == CANT_OPEN_LINKS_CSV && areLinksNecessary )
            WARN << "Unable to open \"links.csv\"\n";

        // ##### The actual parsing ####
        ux lineNumber = 0;
        const char separator = vm.count( "separator" ) ? vm["separator"].as<char>() : '\t';

        for( auto entry : allSentences )
        {
            const sentence & sentence = entry.second;
            const sentence::id id = entry.first;
            const int matchResult = sel.matches( sentence );

            switch( matchResult )
            {
            case SUCCESS:
                if( vm.count( "line-numbers" ) )
                    std::cout << lineNumber++ << separator;

                if( vm.count( "display-ids" ) )
                    std::cout << id << separator;

                std::cout << sentence << "\n";
                break;

            case INTERNAL_ERROR:
                WARN << "[**] Error while parsing sentence: " << sentence << "\n";
                break;

            default:
                break;
            }
        }
    }

    // clean up
    for( auto filter : filtersToDelete )
    {
        delete filter;
    }

    return 0;
}
