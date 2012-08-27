#include "prec.h"
#include "parser.h"
#include "dataset.h"
#include "selecter.h"
#include <boost/regex.hpp>
#include <assert.h>
#include <fstream>
#include <string>
#include <sstream>
#include <functional> //std::mem_fn
#include <unordered_map>

NAMESPACE_START

// The number of different links that links.csv contains (i.e. the number of 
// lines in the file
static const ux MAX_NUMBER_OF_LINK = 4000000;

// The maximum number of tags that a sentence can have
static const ux MAX_NUMBER_OF_TAGS = 10;

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

// ex:
//		10223   less than 5 characters
//
// a digit, followed by some spaces, then a tag name
static const char TAGS_CSV_REGULAR_EXPRESSION[] =
    "^([[:digit:]]+)[[:space:]]+(.*)$";

// __________________________________________________________________________ //

parser::parser( const std::string & _sentences, const std::string & _links, const std::string & _tags )
    :m_sentences( _sentences )
    ,m_links( _links )
    ,m_tags( _tags )
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

int parser::parseLinks()
{
    int ret = SUCCESS;

    std::ifstream file( m_links, std::ios_base::binary|std::ios_base::in );
    
    if( !file.good() )
        ret = CANT_OPEN_TAGS_CSV;
    else
    {
        const boost::regex 	lineRegex( LINKS_CSV_REGULAR_EXPRESSION );
        boost::smatch 		results;

        std::string line;				// will contain the line currently treated
        std::istringstream transform; 	// helps transforming a string into an int

        // parsed values
        sentence::id firstId = 0, secondId = 0;
        std::vector< std::pair<sentence::id, sentence::id> > links;
        links.reserve(MAX_NUMBER_OF_LINK);

        while( std::getline( file, line, '\n' ).good() && !file.eof() )
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
                m_dataMutex.lock();
                sentence & firstSentence = m_output->at( firstId );
                sentence & secondSentence = m_output->at( secondId );
                m_dataMutex.unlock();

                firstSentence.addLink( secondSentence );
            }
            catch( std::out_of_range & oor )
            {
                m_dataMutex.unlock();
                WARN << "Invalid link (" << firstId << "," << secondId << ")\n";
            }
        }
    }

    return ret;
}

// __________________________________________________________________________ //

int parser::parseSentences( )
{
    int ret = SUCCESS;
    
    std::ifstream file( m_sentences, std::ios_base::binary|std::ios_base::in );

    if( !file.good() )
        ret = CANT_OPEN_SENTENCES_CSV;
    else
    {
        const boost::regex 	lineRegex( SENTENCES_CSV_REGULAR_EXPRESSION );

        boost::smatch 		results;

        std::string line;				// will contain the line currently treated
        std::istringstream transform; 	// helps transforming a string into an int

        // parsed values
        sentence::id sentenceId = 0;
        std::string countryCode;
        char sentenceCountry[5];

        /* we want to protect the database of sentences while sentences.csv is
         * parsed. As tags.csv and links.csv might be parsed simultaneously,
         * they could try to fetch sentences that have not yet been parsed from
         * sentences.csv */
        m_dataMutex.lock();                    

        while( std::getline( file, line, '\n' ).good() && !file.eof() )
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

        m_dataMutex.unlock();
    }

    return ret;
}


// -------------------------------------------------------------------------- //

int parser::start()
{
    int ret = SUCCESS;
    ASSERT( m_output != nullptr );
    
    // parsing files
    std::future<int> parseSentence = std::async(std::launch::async, [this]() { return this->parseSentences(); } );
    std::future<int> parseLinksResult = std::async(std::launch::async, [this]() { return this->parseLinks(); } );
    std::future<int> parseTagsResult = std::async(std::launch::async, [this]() { return this->parseTags(); } );

    // retrieving the results
    ret = parseSentence.get();
    const int linksResult = parseLinksResult.get();
    const int tagsResult = parseTagsResult.get();
    
    if (ret == SUCCESS)
        ret = linksResult == SUCCESS ? tagsResult : linksResult;
 
    return ret;
}

// __________________________________________________________________________ //

int parser::parseTags( )
{
    int ret = SUCCESS;

    std::ifstream _file( m_tags, std::ios_base::binary|std::ios_base::in );

    if( !_file.good() )
        ret = CANT_OPEN_TAGS_CSV;
    else
    {
        const boost::regex 	lineRegex( TAGS_CSV_REGULAR_EXPRESSION );
        boost::smatch 		results;
        std::string         line;	// will contain the line currently treated
        std::istringstream  transform; 	// helps transforming a string into an int
        sentence::id        sentenceId = 0; // the id of the sentence
        std::string         tagName; // a temporary variable that will contain
        tagName.reserve( 50 );       // the string of the tag before it is hashed

        // treating the file
        typedef std::vector<sentence::tag> tag_container;
        std::map<sentence::id, tag_container> allTags; // this map will associate an id to a vector of hashes

        while( std::getline( _file, line, '\n' ).good() && !_file.eof() )
        {
            if( regex_match( line, results, lineRegex ) )
            {
                // parsing the first id
                transform.clear();
                std::string && idString = transform.str();
                idString.assign( results[1].first, results[1].second ); // TODO, make it robust
                transform.str( idString );
                transform >> sentenceId;

                // parsing the tag
                tagName.assign( results[2].first, results[2].second );
                const sentence::tag sentenceTag = sentence::getHashFromName( tagName );

                tag_container & currentSentenceTags = allTags[sentenceId];
                currentSentenceTags.reserve(MAX_NUMBER_OF_TAGS);
                currentSentenceTags.push_back(sentenceTag);
            }
            else
            {
                WARN << "[**] line could not be parsed: " << line << "\n";
            }
        }

        // inserting the tags
        for( auto sentenceTag : allTags )
        {
            sentenceId = sentenceTag.first;
            const tag_container & tags = sentenceTag.second;

            try
            {
                m_dataMutex.lock();
                sentence & sentence = m_output->at( sentenceId );
                m_dataMutex.unlock();

                for( auto tag : tags )
                    sentence.addTag( tag );
            }
            catch( std::out_of_range & oor )
            {
                m_dataMutex.unlock();
                WARN << "[**] Invalid tag for sentence of id: " << sentenceId << "\n";
            }
        }
    }

    return ret;
}

NAMESPACE_END
