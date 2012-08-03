#include "prec.h"
#include "parser.h"
#include "dataset.h"
#include "selecter.h"
#include <boost/regex.hpp>
#include <assert.h>
#include <fstream>
#include <string>
#include <sstream>

// ex:
//		1		fra		Bonjour les amis!
//
// a digit, followed by some spaces, then a series of lower characters,
// followed by some more spaces, then the data
static const char CSV_REGULAR_EXPRESSION[] =
    "^([[:digit:]]+)[[:space:]]+([[:lower:]]+|\\\\N)[[:space:]]+(.*)$";

// __________________________________________________________________________ //

parser::parser( const std::string & _file )
    :m_file( _file )
    ,m_output( nullptr )
{
}

// __________________________________________________________________________ //

int parser::setOutput( dataset & _output )
{
    m_output = &_output;
    return 0;
}

// __________________________________________________________________________ //

int parser::start()
{
    ASSERT( m_output != nullptr );

    // opening the file
    std::ifstream file( m_file.c_str(), std::ios_base::binary|std::ios_base::in );

    if( !file.good() )
        return -1;

    const boost::regex 	lineRegex( CSV_REGULAR_EXPRESSION );
    boost::smatch 		results;

    std::string line;				// will contain the line currently treated
    std::istringstream transform; 	// helps transforming a string into an int

    // parsed values
    sentence::id sentenceId = 0;
    std::string countryCode;
    char sentenceCountry[5];

    while( std::getline( file, line, '\n' ).good() && !file.eof() )
    {
        if( regex_match( line, results, lineRegex ) )
        {
            // parsing the id
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

            m_output->push_back( 
                sentence( sentenceId, std::move( sentenceText ), sentenceCountry ) 
            );
        }
        else
        {
            WARN << "[**] line could not be parsed: " << line << "\n";
        }
    }

    return 0;
}


