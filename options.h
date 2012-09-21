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
    userOptions(dataset & _dataset);
    void treatCommandLine( int argc, char * argv[] );
    void getFilters( FilterVector & );
    
    bool isItNecessaryToParseLinksFile() const;
    bool isVerbose() const;
    bool isHelpRequested() const;
    
    void printHelp();

private:
    po::options_description m_desc;
    po::variables_map       m_vm;
    dataset &               m_dataset;
};

inline
bool userOptions::isItNecessaryToParseLinksFile() const
{
    return m_vm.count("is-linked-to") || m_vm.count( "translation-regex" );
}

inline
bool userOptions::isVerbose() const
{
    return m_vm.count("verbose");
}

inline
bool userOptions::isHelpRequested() const
{
    return m_vm.count("help");
}

inline
void userOptions::printHelp()
{
    std::cout << m_desc << std::endl;
}
NAMESPACE_END
#endif //TATOPARSER_OPTIONS_H
