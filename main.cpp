#include "prec.h"
#include "parser.h"
#include "selecter.h"
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/cmdline.hpp>
#include <boost/program_options/parsers.hpp>
#include <iostream>
namespace po = boost::program_options;

cout_warning * gs_coutWarning;

int main( int argc, char * argv[] )
{
    // initializing outputs
    cout_warning warning_output;
    gs_coutWarning = & warning_output;

    // command line parameters
    po::options_description desc( "Allowed options" );
    desc.add_options()
        ( "help,h", "produce help message" )
        ( "compulsory,c", po::value<std::string>(), "The characters that should appear in the sentence" )
        ( "optional,o", po::value<std::string>(), "The characters that may compose the sentence" )
        ( "line-numbers,n", "Display the indexes of the lines" )
        ( "language,l", po::value<std::string>(), "Restrict the languages to a given one" )
        ( "display-ids,i", "Displays the sentence ids" )
        ( "separator,s", po::value<char>(), "Changes the separator characters, default is '\\t'" )
        ( "regex,r", po::value<std::string>(), "A regular expression that the sentence should match entirely" )
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
        VERIFY_EQ( gs_coutWarning->mute(), 0 );

    std::string compulsory, optional;

    // parse the file
    const std::string filename( "sentences.csv" );
    parser tatoeba_parser( filename );

    dataset allSentences;
    VERIFY_EQ( tatoeba_parser.setOutput( allSentences ),    0 );

    const int parseSuccess = tatoeba_parser.start();

    if( parseSuccess == -1 )
        ERR << "Unable to open \"sentences.csv\"\n";
    else
    {
        WARN << "OK, parsed " << allSentences.size() << " sentences\n";
        selecter sel;

        if( vm.count( "optional" ) )
            VERIFY_EQ( sel.canContainCharacters( vm["optional"].as<std::string>().c_str() ), 0 );

        if( vm.count( "compulsory" ) )
            VERIFY_EQ( sel.setMustContainCharacters( vm["compulsory"].as<std::string>() ), 0 );

        if( vm.count( "regex" ) )
        {
            const int regex_result =
                sel.matchRegularExpression( vm["regex"].as<std::string>() );

            switch( regex_result )
            {
            case -1:
                ERR << "Invalid regular expression\n";
                break;

            case -2:
                ERR << "Out of memory\n";
                break;

            default:
                break;
            }

            if( regex_result != 0 )
                return 1;

        }

        std::string language;
        language.reserve( 5 );

        if( vm.count( "language" ) )
        {
            language.assign( vm["language"].as<std::string>() );
            VERIFY_EQ( sel.hasCountryCode( language.c_str() ), 0 );
        }

        ux lineNumber = 0;
        const char separator = vm.count( "separator" ) ? vm["separator"].as<char>() : '\t';

        for( auto sentence : allSentences )
        {
            if( sel.matches( sentence ) == 0 )
            {
                if( vm.count( "line-numbers" ) )
                    std::cout << lineNumber++ << separator;

                if( vm.count( "display-ids" ) )
                    std::cout << sentence.getId() << separator;

                std::cout << sentence << "\n";
            }
        }
    }

    return 0;
}
