#ifndef TATOPARSER_OPTIONS_H
#define TATOPARSER_OPTIONS_H

#include <iostream>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <tatoparser/sentence.h>
#include "filter.h"

NAMESPACE_START

struct dataset;
struct linkset;
struct tagset;
struct listset;

static const char DEFAULT_CONFIG_FILE_PATH[] = "~/.tatoparser";

/**@struct userOptions
 * @brief Treats the user-passed parameters (--help and such)
 */
struct userOptions
{
    /**@brief Constructs an userOptions object */
    userOptions();

    /**@brief Treat the command line arguments argc and argv, populate objects. */
    void treatCommandLine( int argc, char * argv[] );

    /**@brief Populate a list of filters according to the user-specified parameters
     * @param[in] _dataset The list of sentences
     * @param[in] _linkset The list of links
     * @param[in] _tagset The list of tags
     * @param[out] allFilters_ The filter list that will be filled in by the call */
    void getFilters( dataset & _dataset, linkset & _linkset, tagset & _tagset, listset & _listset,
                     FilterVector & allFilters_ );

    /**@brief Checks if any argument the user specified needs the links.csv to be parsed */
    bool isItNecessaryToParseLinksFile() const;

    /**@brief Checks if any argument the user specified needs the tags.csv to be parsed */
    bool isItNecessaryToParseTagFile() const;

    /**@brief Checks if any argument the user specified needs the sentence_detailed.csv to be parsed */
    bool isItNecessaryToParseDetailedFile() const;

    /**@brief Checks if any argument the user specified needs the list.csv to be parsed */
    bool isItNecessaryToParseListFile() const;

    /**@brief Has -v been specified? */
    bool isVerbose() const;

    /**@brief Was --help set? */
    bool isHelpRequested() const;

    /**@brief Was --version set? */
    bool isVersionRequested() const;

    /**@brief Was --orphan set? */
    bool orphansOnly() const;

    /**@brief Was --color set? */
    bool isColored() const;

    /**@brief Was --download set? */
    bool downloadRequested() const;

    /**@brief Should we enumerate lines (like the -n option of 'cat')? */
    bool displayLineNumbers() const;

    /**@brief Should we display the id of the matching sentences? */
    bool displayIds() const;

    /**@brief Should we display the language of the matching sentences? */
    bool displayLanguages() const;

    /**@brief Should we display the first translation? */
    bool displayFirstTranslation() const;

    /**@brief This is debug, only the parsing is done */
    bool justParse() const;

    /**@brief Tells if the user wants to disable parallel processing */
    bool disableParallel() const;

    /**@brief Gets the separator character */
    std::string getSeparator() const;

    /**@brief Gets the name of the list to look the sentence into */
    std::string getListName() const;

    /**@brief Adds filters for a particular command that retrieves direct and indirect translations */
    void treatTranslations( const linkset & _allLinks, FilterVector & allFilters_ );
public:
    void printHelp();
    void printVersion();

    std::string getCsvPath() const;
    std::string getFirstTranslationLanguage() const;

private:
    /**@brief Add filter corresponding to the direct translations of a sentence */
    void addTranslationFilters( sentence::id _id, const linkset & _allLinks, std::vector<sentence::id> & allTranslations_ );

    boost::program_options::options_description m_desc, m_visibleOptions;
    boost::program_options::variables_map       m_vm;
    std::string                                 m_separator;

// ------- CONFIG FILE -------------
public:
    /**@brief Reads user information from a config file
     * @param[in] _path The path to the config file */
    void treatConfigFile();

private:
    /**@brief Sets which options are valid in the config file.*/
    void declareConfigFileValidOptions();

    boost::program_options::options_description     m_configFileDescriptions;
    std::string m_configFileCsvPath;
    std::vector<std::string> m_configFileAcceptedLanguages;
};

// -------------------------------------------------------------------------- //

inline
bool userOptions::isItNecessaryToParseTagFile() const
{
    return m_vm.count( "has-tag" ) > 0;
}

// -------------------------------------------------------------------------- //

inline
bool userOptions::isItNecessaryToParseLinksFile() const
{
    return ( m_vm.count( "is-linked-to" ) > 0 )		||
           ( m_vm.count( "translation-regex" ) > 0 )	||
           ( m_vm.count( "display-first-translation" ) > 0 ) ||
           ( m_vm.count( "is-translatable-in" ) > 0 ) ||
           ( m_vm.count( "translates" ) > 0 );
}

// -------------------------------------------------------------------------- //

inline
bool userOptions::isItNecessaryToParseDetailedFile() const
{
    return m_vm.count( "user" ) > 0 || m_vm.count( "orphan" ) > 0;
}

// -------------------------------------------------------------------------- //

inline
bool userOptions::isItNecessaryToParseListFile() const
{
    return m_vm.count( "in-list" ) > 0;
}

// -------------------------------------------------------------------------- //

inline
bool userOptions::isVerbose() const
{
    return m_vm.count( "verbose" ) > 0;
}

// -------------------------------------------------------------------------- //

inline
bool userOptions::isHelpRequested() const
{
    return m_vm.count( "help" ) > 0;
}

// -------------------------------------------------------------------------- //

inline
bool userOptions::displayLineNumbers() const
{
    return m_vm.count( "display-line-numbers" ) > 0;
}

// -------------------------------------------------------------------------- //

inline
bool userOptions::displayIds() const
{
    return m_vm.count( "display-ids" ) > 0;
}

// -------------------------------------------------------------------------- //

inline
bool userOptions::isVersionRequested() const
{
    return m_vm.count( "version" ) > 0;
}

// -------------------------------------------------------------------------- //

inline
bool userOptions::isColored() const
{
    return m_vm.count( "color" ) > 0;
}

// -------------------------------------------------------------------------- //

inline
bool userOptions::downloadRequested() const
{
    return m_vm.count( "download" ) > 0;
}

// -------------------------------------------------------------------------- //

inline
bool userOptions::orphansOnly() const
{
    return m_vm.count( "orphan" ) > 0;
}

// -------------------------------------------------------------------------- //

inline
void userOptions::printHelp()
{
    std::cout << "Usage: " << PACKAGE_NAME << " <OPTION>\nWhere OPTION can be any of:\n";
    std::cout << m_visibleOptions << '\n';
    std::cout << "Config file will be searched for these options:\n";
    std::cout << m_configFileDescriptions << '\n';
}

// -------------------------------------------------------------------------- //

inline
std::string userOptions::getCsvPath() const
{
    if( m_vm.count( "csv-path" ) )
        return m_vm[ "csv-path" ].as<std::string>();
    else if( m_configFileCsvPath.size() )
        return m_configFileCsvPath;

    //else
    return ".";
}


// -------------------------------------------------------------------------- //
inline
std::string userOptions::getFirstTranslationLanguage() const
{
    if( m_vm.count( "display-first-translation" ) )
        return m_vm[ "display-first-translation" ].as<std::string>();

    return "";
}
// -------------------------------------------------------------------------- //

inline
bool userOptions::justParse() const
{
    return m_vm.count( "just-parse" ) > 0;
}

// -------------------------------------------------------------------------- //

inline
bool userOptions::displayFirstTranslation() const
{
    return m_vm.count( "display-first-translation" ) > 0;
}

// -------------------------------------------------------------------------- //

inline
bool userOptions::displayLanguages() const
{
    return m_vm.count( "display-lang" ) > 0;
}
// -------------------------------------------------------------------------- //

inline
std::string userOptions::getSeparator() const
{
    return m_separator;
}
// -------------------------------------------------------------------------- //

inline
std::string userOptions::getListName() const
{
    return m_vm.count( "in-list" ) > 0 ? m_vm[ "in-list" ].as<std::string>() : "";
}
// -------------------------------------------------------------------------- //

inline
bool userOptions::disableParallel() const
{
    return m_vm.count( "disable-parallel" ) > 0;
}
NAMESPACE_END
#endif //TATOPARSER_OPTIONS_H
