#include "prec.h"
#include "options.h"
#include "sentence.h"
#include "filter_regex.h"
#include "filter_translation_regex.h"
#include "filter_link.h"
#include "filter_lang.h"
#include "dataset.h"
#include <boost/program_options/cmdline.hpp>
#include <boost/program_options/parsers.hpp>

NAMESPACE_START

userOptions::userOptions()
    :m_desc( "" )
    ,m_vm()
{
    m_desc.add_options()
    ( "help,h", "produce help message" )
    //( "compulsory,c", po::value<std::string>(), "The characters that should appear in the sentence." )
    //( "optional,o", po::value<std::string>(), "The characters that may compose the sentence." )
    ( "display-line-numbers,n", "Display the indexes of the lines." )
    ( "language,l", po::value<std::string>(), "Restrict the languages to a given one." )
    ( "display-ids,i", "Displays the sentence ids." )
    //( "separator,s", po::value<char>(), "Changes the separator characters, default is '\\t'" )
    ( "regex,r", po::value<std::vector<std::string> >()->composing(), "A regular expression that the sentence should match entirely." )
    ( "is-linked-to", po::value<sentence::id>(), "Filters only sentences that are a translation of the given id." )
    //( "depedent-regex,d",  po::value<std::vector<std::string> >()->composing(), "A set of regular expressions which have to be all matched." )
    //( "translatable-in,t", po::value<std::string>(), "A language that the sentence can be translated into." )
    //( "translation-contains-regex,j", po::value<std::string>(), "A regex that one of the translation of the sentence should match." )
    ( "verbose,v", "Displays more info" )
    //( "has-tag,g", po::value<sentence::tag>(), "Checks if the sentence has a given tag" )
    //( "translates", po::value<sentence::id>(), "Checks if the sentence is a translation of the given sentence id" )
    ( "translation-regex,p", po::value<std::vector<std::string> >()->composing(),
      "Filters only sentences which translations match this regex. If many "
      "regular expressions are provided, a sentence will be kept if any of its "
      "translations matches them all" )
    ( "version", "Displays the current version of the program")
    ;
}

void userOptions::treatCommandLine( int argc, char * argv[] )
{
    po::store( po::parse_command_line( argc, argv, m_desc ), m_vm );
    po::notify( m_vm );
}

void userOptions::getFilters( dataset & _dataset, FilterVector & allFilters_ )
{
    qlog::warning( allFilters_.size() ) << "allFilters.size() > 0\n";

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
                new filterLink( _dataset, m_vm["is-linked-to"].as<sentence::id>() )
            )
        );
    }

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
                new filterTranslationRegex( _dataset, m_vm["translation-regex"].as<std::vector<std::string>>() )
            )
        );
    }

    qlog::info << "registered " << allFilters_.size() << " filters\n";
}

void userOptions::printVersion()
{
    std::cout   << PACKAGE_STRING << "\n"
                << "Please report bugs to " << PACKAGE_BUGREPORT << "\n";
}

NAMESPACE_END
