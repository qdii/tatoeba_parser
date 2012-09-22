#ifndef FILTER_REGEX
#define FILTER_REGEX

#include <boost/regex/icu.hpp>
#include <boost/regex.hpp>
#include "filter.h"

NAMESPACE_START

struct filterRegex : public filter
{
    filterRegex( const std::string & _regex ) :m_compiledRegex( boost::make_u32regex( _regex ) ) { }
    
    bool parse(const sentence & _sentence) throw() override
    {
        return boost::u32regex_match( _sentence.str(), m_compiledRegex );
    }

private:
    boost::u32regex m_compiledRegex;
};

NAMESPACE_END

#endif // FILTER_REGEX
