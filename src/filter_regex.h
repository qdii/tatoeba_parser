#ifndef FILTER_REGEX
#define FILTER_REGEX

#include <boost/regex/icu.hpp>
#include <boost/regex.hpp>
#include <tatoparser/sentence.h>
#include "filter.h"

NAMESPACE_START

/**@struct filterRegex
 * @brief Checks that a sentence matches a regular expression */
struct filterRegex : public filter
{
    /**@brief Construct a filterRegex
     * @throw boost::regex_error If the regular expression is invalid
     * @param[in] _regex The regular expression to match the sentence against
     * @param[in] _cs A boolean set to true if the match is case-sensitive */
    explicit
    filterRegex( const std::string & _regex, bool _cs = true )
        :m_compiledRegex( boost::make_u32regex( _regex, _cs ? boost::regex_constants::normal : boost::regex_constants::normal | boost::regex_constants::icase ) )
    {
    }

    /**@brief Checks that a sentence matches the regular expression
       @param[in] _sentence The sentence to check against the regular expression
       @throw boost::regex_error If the regular expression causes an error (overflow).
       @return true if the sentence matches */
    bool parse( const sentence & _sentence ) TATO_OVERRIDE
    {
        return boost::u32regex_match( _sentence.str(), m_compiledRegex );
    }

private:
    boost::u32regex m_compiledRegex;
};

NAMESPACE_END

#endif // FILTER_REGEX
