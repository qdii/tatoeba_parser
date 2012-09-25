#include "prec.h"
#include "dataset.h"
#include "options.h"
#include "filter_id.h"
#include "filter_regex.h"
#include "fast_sentence_parser.h"
#include "fast_link_parser.h"
#include <iostream>

void startLog(bool _verbose);
bool areUserOptionsValid();
int treatUserOptions( FilterVector & _allFilters );
int parseFile( parser & _parser, dataset & data_ );
int main( int argc, char * argv[] )
{
    // reserving data for the sentences
    dataset data;

    // creating the filters from command line
    FilterVector allFilters;
    allFilters.reserve( 5 );
    userOptions options( data );
    options.treatCommandLine( argc, argv );
    startLog(options.isVerbose());

    try
    {
        options.getFilters( allFilters );
    }
    catch (const boost::regex_error & err)
    {
        qlog::error << "Invalid regular expression\n"
                    << qlog::setBashColor() << err.what() << '\n';
        return 0;
    }

    if( !allFilters.size() || options.isHelpRequested() )
    {
        options.printHelp();
        return 0;
    }


    // parsing sentences.csv
    fileMapper sentenceMap( "sentences.csv" );
    fastSentenceParser sentenceParser( sentenceMap.getRegion(), sentenceMap.getRegion() + sentenceMap.getSize() );
    const int nbLines = parseFile( sentenceParser, data );

    if( nbLines == 0 )
        return 0;

    // parsing links.csv
    if( options.isItNecessaryToParseLinksFile() )
    {
        fileMapper linksMap( "links.csv" );
        fastLinkParser linksParser( linksMap.getRegion(), linksMap.getRegion() + linksMap.getSize() );
        parseFile( linksParser, data );
    }

    data.prepare();

    auto itr = data.begin();
    auto endFilter = allFilters.end();
    unsigned printedLineNumber = 0;

    for( int i = 0; i < nbLines; ++i )
    {
        auto sentence = *itr++;
        bool shouldDisplay = true;

        for( auto filter = allFilters.begin(); shouldDisplay && filter != endFilter; ++filter )
        {
            shouldDisplay &= ( *filter )->parse( sentence );
        }

        if( shouldDisplay )
        {
            if (options.displayLineNumbers())
                std::cout << ++printedLineNumber << '\t';
            if (options.displayIds())
                std::cout << sentence.getId() << '\t';

            std::cout << sentence.str() << '\n';
        }
    }
    return 0;
}


int parseFile( parser & _parser, dataset & data_ )
{
    _parser.setOutput( data_ );
    return _parser.start();
}

void startLog(bool verbose)
{
    qlog::setLogLevel( verbose ? qlog::Loglevel::info : qlog::Loglevel::error );
    qlog::setPrependedTextQdiiFlavourBashColors();
    qlog::setOutput( std::cout );
}
