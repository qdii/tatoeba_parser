#include "prec.h"
#include "dataset.h"
#include "linkset.h"
#include "tagset.h"
#include "options.h"
#include "filter_id.h"
#include "filter_tag.h"
#include "filter_regex.h"
#include "fast_sentence_parser.h"
#include "fast_link_parser.h"
#include "fast_tag_parser.h"
#include "file_mapper.h"
#include <iostream>

USING_NAMESPACE

static const char SENTENCES_FILENAME[] = "sentences.csv";
static const char LINKS_FILENAME[] = "links.csv";
static const char TAG_FILENAME[] = "tags.csv";

void startLog( bool _verbose );

/// How does it work?
///
/// Basically, we check the options the user set through the different program
/// parameters (like "--language fra"), then we parse the different csv files,
/// we create a list of filters and we filter all the sentences based on
/// those filters.

int main( int argc, char * argv[] )
{
    FilterVector allFilters; ///< the filter list
    ///< to select sentences based on the user options)

    // create the filters with respect to the user options
    allFilters.reserve( 5 );
    userOptions options;
    options.treatCommandLine( argc, argv );
    startLog( options.isVerbose() );
    options.treatConfigFile();
    const std::string csvPath = options.getCsvPath();

    datainfo info; ///< info will store statistics about the files (how many sentences there are, etc.)
    linkset allLinks;
    tagset allTags;
    dataset allSentences; ///< data will contain the sentences

    try
    {
        options.getFilters( allSentences, allLinks, allTags, allFilters );
    }
    catch( const boost::regex_error & err )
    {
        qlog::error << "Invalid regular expression\n"
                    << qlog::setBashColor() << err.what() << '\n';
        return 0;
    }

    if( options.isVersionRequested() )
    {
        options.printVersion();
        return 0;
    }

    if( ( !allFilters.size() && !options.justParse() ) || options.isHelpRequested() )
    {
        options.printHelp();
        return 0;
    }



    ////////////////////////////
    // parsing sentences file //
    ////////////////////////////

    fileMapper * sentenceMap = nullptr;
    const std::string sentencesPath = csvPath + '/' + SENTENCES_FILENAME;

    // map "sentences.csv" to some address in our virtual space
    try
    {
        sentenceMap = new fileMapper( sentencesPath );
    }
    catch( const invalid_file & exception )
    {
        qlog::error << "Cannot open " << sentencesPath << '\n';
        return 0;
    }
    catch( const map_failed & exception )
    {
        qlog::error << "Failed to map " << sentencesPath << '\n';
        return 0;
    }

    // create the parser
    fastSentenceParser<char *> sentenceParser(
        sentenceMap->begin(),
        sentenceMap->end()
    );

    // allocate memory for the sentence structure
    info.m_nbSentences = sentenceParser.countLines();

    if( info.m_nbSentences <= 0 )
    {
        qlog::error << sentencesPath << " is empty\n";
        return 0;
    }

    try
    {
        allSentences.allocate(info);
    }
    catch (const std::bad_alloc & )
    {
        qlog::error << "Not enough memory.\n";
        return 0;
    }

    // start parsing
    info.m_nbSentences = sentenceParser.start( allSentences );

    // retrieve the sentence of highest id so as to be able to create containers
    // of the right size to store links and tags
    const sentence & sentenceOfHighestId =
        *std::max_element(
            allSentences.begin(), allSentences.end(),
            []( const sentence & _a, const sentence & _b ) { return _a.getId() < _b.getId(); }
        );

    qlog::info << "highest id: " << sentenceOfHighestId.getId() << '\n';
    info.m_highestId = sentenceOfHighestId.getId();




    ///////////////////////
    // parsing links.csv //
    ///////////////////////

    if( options.isItNecessaryToParseLinksFile() )
    {
        const std::string linksPath = csvPath + '/' + LINKS_FILENAME;

        try
        {
            // we map tags.csv to somewhere in our virtual space
            fileMapper linksMap( linksPath );

            // we create the parser
            fastLinkParser<char *> linksParser(linksMap.begin(), linksMap.end());

            // we allocate memory for the structure that will store the links
            info.m_nbLinks = linksParser.countLines();
            allLinks.allocate( info );

            // we parse the file and store the data
            linksParser.start( allLinks );
        }
        catch( const invalid_file & exception )
        {
            qlog::error << "Cannot open " << linksPath << '\n';
            return 0;
        }
        catch( const map_failed & exception )
        {
            qlog::error << "Failed to map " << linksPath << '\n';
            return 0;
        }
        catch ( const std::bad_alloc & exception )
        {
            qlog::error << "Out of memory\n";
            return 0;
        }
    }



    //////////////////////
    // parsing tags.csv //
    //////////////////////

    fileMapper * tagFileMapping = nullptr;

    if( options.isItNecessaryToParseTagFile() )
    {
        const std::string tagsPath = csvPath + '/' + TAG_FILENAME;
        try
        {
            tagFileMapping = new fileMapper( tagsPath );
            fastTagParser<char *> tagParser(
                tagFileMapping->begin(),
                tagFileMapping->end()
            );
            tagParser.start(allTags);
        }
        catch( const std::bad_alloc & exc )
        {
            qlog::error << "An error occurred while parsing file " << tagsPath << std::endl;
            return 0;
        }
        catch( const invalid_file & exception )
        {
            qlog::error << "Cannot open " << tagsPath << '\n';
            return 0;
        }
        catch( const map_failed & exception )
        {
            qlog::error << "Failed to map " << tagsPath << '\n';
            return 0;
        }
    }




    ///////////////////////////
    //  filtering sentences  //
    ///////////////////////////
    if( !options.justParse() )
    {
        // create an container to retrieve sentences from id in a very fast manner
        try
        {
            allSentences.prepare( info );
        }
        catch( const std::bad_alloc & exc)
        {
            qlog::error << "Not enough memory.\n";
            return 0;
        }

        // go through every sentence and see if it matches the filter
        auto itr = allSentences.begin();
        auto endFilter = allFilters.end();
        unsigned printedLineNumber = 0;
        std::string translationLanguage = options.getFirstTranslationLanguage();

        for( size_t i = 0; i < info.m_nbSentences; ++i )
        {
            auto sentence = *itr++;
            bool shouldDisplay = true;

            for( auto filter = allFilters.begin(); shouldDisplay && filter != endFilter; ++filter )
            {
                shouldDisplay &= ( *filter )->parse( sentence );
            }

            // display the sentence if it is seleted by all the filters
            if( shouldDisplay )
            {
                // option -n
                if( options.displayLineNumbers() )
                    std::cout << ++printedLineNumber << '\t';

                // option -i
                if( options.displayIds() )
                    std::cout << sentence.getId() << '\t';

                // display the sentence
                std::cout << sentence.str();

                // display a translation if it was requested
                if ( options.displayFirstTranslation() )
                {
                    // find a suitable translation
                    const sentence::id firstTranslationId =
                        getFirstSentenceTranslation(
                            allSentences,
                            allLinks,
                            sentence.getId(),
                            translationLanguage
                        );

                    if ( firstTranslationId != sentence::INVALID_ID
                            && allSentences[firstTranslationId] )
                    {
                        // display the translation
                        std::cout << '\t' << allSentences[firstTranslationId]->str();
                    }
                }

                std::cout << '\n';
            }
        }
    }

    delete sentenceMap;
    delete tagFileMapping;

    return 0;
}

// -------------------------------------------------------------------------- //

void startLog( bool verbose )
{
    qlog::setLogLevel( verbose ? qlog::Loglevel::info : qlog::Loglevel::error );
    qlog::setPrependedTextQdiiFlavourBashColors();
    qlog::setOutput( std::cout );
}
