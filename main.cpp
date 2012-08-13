#include "prec.h"
#include "parser.h"
#include "selecter.h"
#include "filter_regex.h"
#include "filter_language.h"
#include "filter_translation.h"
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/cmdline.hpp>
#include <boost/program_options/parsers.hpp>
#include <iostream>

namespace po = boost::program_options;

NAMESPACE_START

cout_warning * gs_coutWarning;

NAMESPACE_END
USING_NAMESPACE

int addRegularExpressionFilter( const std::string & _regex, Filter * & filter_ );

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
        ( "regex,r", po::value<std::string>(), "A regular expression that the sentence should match entirely." )
        ( "translatable-in,t", po::value<std::string>(), "A language that the sentence can be translated into." )
        ( "translation-contains-regex,j", po::value<std::string>(), "A regex that one of the translation of the sentence should match." )
        ( "verbose,v", "Displays warnings" )
    ;

    po::variables_map vm;
    po::store( po::parse_command_line( argc, argv, desc ), vm );
    po::notify( vm );
    
    if( vm.count( "help" ) )
    {
        std::cout << desc << "\n";
        return 1;
    }

    if( !vm.count( "verbose" ) )
        VERIFY_EQ( gs_coutWarning->mute(), SUCCESS );

    // parse the files
    const std::string sentencesFile( "sentences.csv" );
    const std::string linksFile( "links.csv" );
    parser tatoeba_parser( sentencesFile, linksFile );

    dataset allSentences;
    VERIFY_EQ( tatoeba_parser.setOutput( allSentences ),    SUCCESS );

    // ###### SET UP THE FILTERS #####
    selecter sel;
    std::vector< Filter * > filtersToDelete;

    // LANGUAGE FILTER
    if( vm.count( "language" ) )
    {
        FilterLanguage * const filter = new FilterLanguage( std::move( vm["language"].as<std::string>() ) );

        if( filter )
        {
            filtersToDelete.push_back( filter );
            VERIFY_EQ( sel.addFilter( *filter ), SUCCESS );
        }
        else
        {
            ERR << "Out of memory\n";
            return 1;
        }
    }

    // OPTIONAL CHARACTERS
    if( vm.count( "optional" ) )
    {
        Filter * filter = nullptr;

        if( addRegularExpressionFilter( std::string( "[" + vm["optional"].as<std::string>() + "]*" ), filter ) == SUCCESS )
        {
            ASSERT( filter != nullptr );
            filtersToDelete.push_back( filter );
            VERIFY_EQ( sel.addFilter( *filter ), SUCCESS );
        }
        else
            return 1;
    }

    // MANDATORY CHARACTERS
    if( vm.count( "compulsory" ) )
    {
        Filter * filter = nullptr;

        if( addRegularExpressionFilter( std::string( ".*[" + vm["compulsory"].as<std::string>() + "]+.*" ), filter ) == SUCCESS )
        {
            ASSERT( filter != nullptr );
            filtersToDelete.push_back( filter );
            VERIFY_EQ( sel.addFilter( *filter ), SUCCESS );
        }
        else
            return 1;
    }


    // USER DEFINED REGEX
    if( vm.count( "regex" ) )
    {
        Filter * filter = nullptr;

        if( addRegularExpressionFilter( std::string( vm["regex"].as<std::string>() ), filter ) == SUCCESS )
        {
            ASSERT( filter != nullptr );
            filtersToDelete.push_back( filter );
            VERIFY_EQ( sel.addFilter( *filter ), SUCCESS );
        }
        else
            return 1;
    }

    // FILTERS ON TRANSLATIONS
    if( vm.count( "translatable-in" ) || vm.count( "translation-contains-regex" ) )
    {
        FilterTranslation * const filterTranslation = new FilterTranslation;

        if( filterTranslation )
        {
            filtersToDelete.push_back( filterTranslation );

            // language
            if( vm.count( "translatable-in" ) )
            {
                FilterLanguage * const filterLanguage = new FilterLanguage( std::move( vm["translatable-in"].as<std::string>() ) );

                if( filterLanguage )
                {
                    filtersToDelete.push_back( filterLanguage );
                    VERIFY_EQ( filterTranslation->addFilter( *filterLanguage ), 0 );
                }
                else
                    ERR << "Out of memory.\n";
            }


            // regex
            if( vm.count( "translation-contains-regex" ) )
            {
                Filter * filterRegex = nullptr;

                if( addRegularExpressionFilter( std::string( vm["translation-contains-regex"].as<std::string>() ), filterRegex ) == SUCCESS )
                {
                    ASSERT(filterRegex);
                    filtersToDelete.push_back( filterRegex );
                    filterTranslation->addFilter( *filterRegex );
                }
            }

            VERIFY_EQ( sel.addFilter( *filterTranslation ), SUCCESS );
        }
    }

    // START PARSING
    const int parseSuccess = tatoeba_parser.start();

    if( parseSuccess == CANT_OPEN_SENTENCES_CSV )
        ERR << "Unable to open \"sentences.csv\"\n";
    else
    {
        WARN << "OK, parsed " << allSentences.size() << " sentences\n";

        if( parseSuccess == CANT_OPEN_LINKS_CSV )
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

// __________________________________________________________________________ //

int addRegularExpressionFilter( const std::string & _regex, Filter *& filter_ )
{
    int ret = SUCCESS;

    FilterRegex * const regex = new FilterRegex( _regex );

    if( regex != nullptr )
    {
        if( !regex->isRegexValid() )
        {
            ERR << "Invalid regular expression\n";
            ret = INVALID_ARG;
        }
        else
            filter_ = regex;
    }
    else
    {
        ERR << "Out of memory\n";
        ret = OUT_OF_MEMORY;
    }

    return ret;
}
