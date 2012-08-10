#include "prec.h"
#include "parser.h"
#include "dataset.h"
#include "selecter.h"
#include <boost/regex.hpp>
#include <assert.h>
#include <fstream>
#include <string>
#include <sstream>

NAMESPACE_START

// ex:
//		1		fra		Bonjour les amis!
//
// a digit, followed by some spaces, then a series of lower characters,
// followed by some more spaces, then the data
static const char SENTENCES_CSV_REGULAR_EXPRESSION[] =
    "^([[:digit:]]+)[[:space:]]+([[:lower:]]+|\\\\N)[[:space:]]+(.*)$";

// __________________________________________________________________________ //

// ex:
//		1		10223
//
// a digit, followed by some spaces, then another digit
static const char LINKS_CSV_REGULAR_EXPRESSION[] =
    "^([[:digit:]]+)[[:space:]]+([[:digit:]]+)$";

// __________________________________________________________________________ //

parser::parser( const std::string & _sentences, const std::string & _links )
    :m_sentences( _sentences )
    ,m_links( _links )
    ,m_output( nullptr )
{
}

// __________________________________________________________________________ //

int parser::setOutput( dataset & _output )
{
    m_output = &_output;
    return SUCCESS;
}

// __________________________________________________________________________ //

int parser::parseLinks( std::ifstream & _file )
{
    const boost::regex 	lineRegex( LINKS_CSV_REGULAR_EXPRESSION );
    boost::smatch 		results;

    std::string line;				// will contain the line currently treated
    std::istringstream transform; 	// helps transforming a string into an int

    // parsed values
    sentence::id firstId = 0, secondId = 0;
    std::vector< std::pair<sentence::id, sentence::id> > links;

    while( std::getline( _file, line, '\n' ).good() && !_file.eof() )
    {
        if( regex_match( line, results, lineRegex ) )
        {
            // parsing the first id
            {
                transform.clear();
                std::string && idString = transform.str();
                idString.assign( results[1].first, results[1].second );
                transform.str( idString );
                transform >> firstId;
            }

            // parsing the second id
            {
                transform.clear();
                std::string && idString = transform.str();
                idString.assign( results[2].first, results[2].second );
                transform.str( idString );
                transform >> secondId;
            }

            links.push_back(
                std::pair<sentence::id, sentence::id>( firstId, secondId )
            );
        }
        else
        {
            WARN << "[**] link could not be parsed: " << line << "\n";
        }
    }

    for( auto link : links )
    {
        firstId = link.first;
        secondId = link.second;

        // retrieving the sentences
        try 
        {
            sentence & firstSentence = m_output->at(firstId);
            sentence & secondSentence = m_output->at(secondId);
            
            firstSentence.addLink( secondSentence );
        }
        catch (std::out_of_range & oor)
        {
            WARN << "Invalid link (" << firstId << "," << secondId << ")\n";
        }            
    }

    return SUCCESS;
}

// __________________________________________________________________________ //

int parser::parseSentences( std::ifstream & _file )
{
    const boost::regex 	lineRegex( SENTENCES_CSV_REGULAR_EXPRESSION );
    boost::smatch 		results;

    std::string line;				// will contain the line currently treated
    std::istringstream transform; 	// helps transforming a string into an int

    // parsed values
    sentence::id sentenceId = 0;
    std::string countryCode;
    char sentenceCountry[5];

    while( std::getline( _file, line, '\n' ).good() && !_file.eof() )
    {
        if( regex_match( line, results, lineRegex ) )
        {
            // parsing the first id
            transform.clear();
            std::string && idString = transform.str();
            idString.assign( results[1].first, results[1].second );
            transform.str( idString );
            transform >> sentenceId;

            // parsing the country code
            countryCode.assign( results[2].first, results[2].second );
            memset( sentenceCountry, 0, 5 );
            countryCode.copy( sentenceCountry, 4, 0 );

            // parsing the sentence data
            std::string sentenceText( results[3].first, results[3].second );

            m_output->insert( 
                std::pair< const sentence::id, sentence >( 
                    sentenceId, sentence( sentenceId, std::move( sentenceText ), sentenceCountry ) 
                ) 
                );
        }
        else
        {
            WARN << "[**] line could not be parsed: " << line << "\n";
        }
    }

    return SUCCESS;
}

// __________________________________________________________________________ //


int parser::start()
{
    int ret = SUCCESS;
    ASSERT( m_output != nullptr );

    // opening the file
    std::ifstream file( m_sentences.c_str(), std::ios_base::binary|std::ios_base::in );

    if( !file.good() )
        ret = CANT_OPEN_SENTENCES_CSV;
    else
    {
        // parsing sentences.csv
        ret = parseSentences( file );
        file.close();

        if( ret == SUCCESS && m_links != "" )
        {
            // parsing links.csv
            file.open( m_links.c_str(), std::ios_base::binary|std::ios_base::in );

            if( !file.good() )
                ret = CANT_OPEN_LINKS_CSV;
            else
            {
                ret = parseLinks( file );
                file.close();
            }
        }
    }

    return ret;
}


NAMESPACE_END
