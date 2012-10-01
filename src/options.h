#ifndef TATOPARSER_OPTIONS_H
#define TATOPARSER_OPTIONS_H

#include <iostream>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include "filter.h"


NAMESPACE_START

namespace po = boost::program_options;
struct dataset;
struct userOptions
{
    userOptions();
    void treatCommandLine( int argc, char * argv[] );
    void getFilters( dataset &, FilterVector & );

    bool isItNecessaryToParseLinksFile() const;
    bool isItNecessaryToParseTagFile() const;
    bool isVerbose() const;
    bool isHelpRequested() const;
    bool isVersionRequested() const;
    bool displayLineNumbers() const;
    bool displayIds() const;
    bool justParse() const;

    void printHelp();
    void printVersion();

    std::string getCsvPath() const;

private:
    po::options_description m_desc;
    po::variables_map       m_vm;
};

inline
bool userOptions::isItNecessaryToParseTagFile() const
{
    return m_vm.count( "has-tag" );
}

inline
bool userOptions::isItNecessaryToParseLinksFile() const
{
    return  m_vm.count( "is-linked-to" ) ||
            m_vm.count( "translation-regex" ) ||
            m_vm.count( "is-translatable-in" );
}

inline
bool userOptions::isVerbose() const
{
    return m_vm.count( "verbose" );
}

inline
bool userOptions::isHelpRequested() const
{
    return m_vm.count( "help" );
}

inline
bool userOptions::displayLineNumbers() const
{
    return m_vm.count( "display-line-numbers" );
}

inline
bool userOptions::displayIds() const
{
    return m_vm.count( "display-ids" );
}

inline
bool userOptions::isVersionRequested() const
{
    return m_vm.count( "version" );
}

inline
void userOptions::printHelp()
{
    std::cout << "Usage: " << PACKAGE_NAME << " <OPTION>\nWhere OPTION can be any of:\n";
    std::cout << m_desc << std::endl;
}

inline
std::string userOptions::getCsvPath() const
{
    return m_vm.count( "csv-path" ) ? m_vm["csv-path"].as<std::string>() : ".";
}

inline
bool userOptions::justParse() const
{
    return m_vm.count( "just-parse" );
}
NAMESPACE_END
#endif //TATOPARSER_OPTIONS_H
