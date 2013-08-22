#ifndef FILTER_FUZZY_H
#define FILTER_FUZZY_H

NAMESPACE_START

#include "filter.h"
#include <algorithm> // copy_if
#include <limits> // numeric limits
#include <iterator> // back_inserter
#include <vector>
#include <cctype>
#include <boost/regex.hpp>

typedef unsigned int lvh_distance;
static constexpr lvh_distance INFINITE_DISTANCE = std::numeric_limits<lvh_distance>::max();

// taken from: http://en.wikibooks.org/wiki/Algorithm_Implementation/Strings/Levenshtein_distance#C.2B.2B

/**@brief Computes the Levenshtein distance between two strings
 * @param[in] s1 The first string
 * @param[in] s2 The second string
 * @return A levenshtein distance. */
static
lvh_distance levenshtein_distance( const std::string & s1, const std::string & s2 )
{
    const size_t len1 = s1.size(), len2 = s2.size();
    std::vector<unsigned int> col( len2+1 ), prevCol( len2+1 );

    const size_t prevColSize = prevCol.size();
    for( unsigned int i = 0; i < prevColSize; i++ )
        prevCol[i] = i;

    for( unsigned int i = 0, j; i < len1; ++i )
    {
        col[0] = i+1;
        const char s1i = s1[i];
        for( j = 0; j < len2; ++j )
        {
            const auto minPrev = 1 + std::min( col[j], prevCol[1 + j] );
            col[j+1] = std::min( minPrev, prevCol[j] + ( static_cast<unsigned int>( s1i != s2[j] ) ) );
        }
        col.swap( prevCol );
    }
    return prevCol[len2];
}

// -------------------------------------------------------------------------- //

/**@brief Splits a string according to a delimiter and returns a container of them
 * @param[in] str The string to split
 * @param[in] delimiter The delimiter character
 * @return A container containing the substrings */
template < class ContainerT > static
ContainerT split( const std::string & str, char delimiter = ' ' )
{
    static ContainerT tokens;
    tokens.clear();
    typedef typename ContainerT::value_type ValueType;
    typedef typename ValueType::size_type SizeType;

    std::string::size_type pos, lastPos = 0;
    while( true )
    {
        pos = str.find( delimiter, lastPos );
        if TATO_LIKELY( pos != std::string::npos )
        {
            if TATO_LIKELY( pos != lastPos )
                tokens.push_back( ValueType( str.data()+lastPos, static_cast< SizeType >( pos-lastPos ) ) );
        }
        else
        {
            pos = str.length();

            if TATO_LIKELY( pos != lastPos )
                tokens.push_back( ValueType( str.data()+lastPos, static_cast< SizeType >( pos-lastPos ) ) );

            break;
        }

        lastPos = pos + 1;
    }

    return tokens;
}

// -------------------------------------------------------------------------- //

/**
 * @desc Replace punctuation with white spaces
 * @param[in] s The string to remove punctuation signs from
 * @return A string without punctuation signs
 */
static
std::string removePunctuation( const std::string & s )
{
    std::string ret;
    ret.reserve( s.size() );

    std::copy_if(
        s.begin(), s.end(), std::back_inserter( ret ),
        []( char c )
        {
            return ispunct( static_cast<int>( c ) ) == 0;
        }
    );

    assert(ret.size() <= s.size()); // punctuation signs are now deleted, if any

    return ret;
}

// -------------------------------------------------------------------------- //
/**
 * @desc Computes the minimum distance from a word to a sentence's words
 * @param[in] match The word to match
 * @param[in] sentence A sentence
 * @return lvh_distance The minimum distance from a word to any of the sentence's words
 *
 * A sentence is composed of words. This function computes the distances of a match word to
 * any of the sentence's words and return the smallest distance.
 */
template<class T> static
lvh_distance levenshtein_distance_word( const T & match, const T & sentence )
{
    if( !sentence.size() )
        return INFINITE_DISTANCE;

    assert( match.size() );

    // split the sentence into words, and we take the smallest levenstein distance
    std::vector<std::string> words = split<std::vector<std::string> >( sentence );
    assert( words.size() );

    std::vector< lvh_distance > distances;
    distances.reserve( words.size() );

    // compute the levnshtein distance for each word
    std::for_each( words.begin(), words.end(),
                   [&match, &distances]( std::string & word )
                    {
                        distances.push_back( levenshtein_distance( match,word ) );
                    }
                 );
    assert( distances.size() );

    // return the smallest one
    const auto smallestElement = std::min_element( distances.begin(), distances.end() );
    assert( distances.end() != smallestElement );
    return *smallestElement;
}

// -------------------------------------------------------------------------- //

struct fuzzyFilterOption
{
    std::string expression;
    unsigned int numberOfMatch;
};

// -------------------------------------------------------------------------- //

struct filterFuzzy : public filter
{
    filterFuzzy( const std::string & _expression, unsigned int _nbSentencesToKeep )
        :m_expression( _expression )
        ,m_keptSentences()
        ,m_levenshteinValues()
    {
        m_keptSentences.reserve( _nbSentencesToKeep );
        m_levenshteinValues.reserve( _nbSentencesToKeep );
    }

    filterFuzzy( const fuzzyFilterOption & _options )
        :filterFuzzy( _options.expression, _options.numberOfMatch )
    {
    }

    /**@brief Checks that a sentence is close enough
    * @param[in] _sentence The second sentence.
    * @return True if the passed sentence is linked to the sentence passed in
    * the filterLink constructor */
    bool parse( const sentence & _sentence ) throw() TATO_OVERRIDE
    {
        using namespace qlog;

        assert( m_levenshteinValues.size() == m_keptSentences.size() );
        assert( m_levenshteinValues.capacity() == m_keptSentences.capacity() );

        const lvh_distance distance = levenshtein_distance_word( m_expression, removePunctuation( _sentence.str() ) );
        qlog::warning( INFINITE_DISTANCE == distance ) << "Skipping invalid sentence: " << _sentence.getId() << '\t' << _sentence.str() << '\n';
        //qlog::info << "\tdistance of " << color(green) << _sentence.str() << color() << " and " << color(green) << m_expression << color() << ": " << distance << '\n';
        unsigned int maxIndex = 0;
        lvh_distance maxDistance = 0, minDistance = std::numeric_limits<lvh_distance>::max();

        // if the top N values have not been computed then we add one more
        if( m_levenshteinValues.size() < m_levenshteinValues.capacity() )
        {
            // qlog::info << "\tadding distance: " << distance << '\n';
            m_levenshteinValues.push_back( distance );
            m_keptSentences.push_back( _sentence.getId() );
        }
        else
        {
            for( unsigned int index = 0 ; index <  m_levenshteinValues.size() ; ++index )
            {
                if( m_levenshteinValues[index] > maxDistance )
                {
                    maxDistance = m_levenshteinValues[index];
                    maxIndex = index;
                }

                if( m_levenshteinValues[index] < minDistance )
                {
                    minDistance = m_levenshteinValues[index];
                }
            }

            if( distance <= minDistance )
            {
                // qlog::info << "\treplacing distance: " << m_levenshteinValues[maxIndex]  << " with " << distance << '\n';
                m_levenshteinValues[maxIndex] = distance;
                m_keptSentences[maxIndex] = _sentence.getId();
            }
        }

        return true;
    }

    bool postProcess( const sentence & _sentence ) TATO_OVERRIDE
    {
        return std::find( m_keptSentences.begin(), m_keptSentences.end(), _sentence.getId() ) != m_keptSentences.end();
    }

private:
    const std::string & m_expression;
    std::vector< sentence::id > m_keptSentences;
    std::vector< lvh_distance > m_levenshteinValues;
};

NAMESPACE_END

#endif // FILTER_FUZZY_H
