#ifndef FILTER_REGEX
#define FILTER_REGEX

#include "prec.h"
#include "filter.h"
#include <string>
#include <unicode/regex.h>

NAMESPACE_START

/**@struct FilterRegex
 * @brief Checks whether a sentence matches a regular expression
 */
struct FilterRegex : public Filter
{
    FilterRegex();
    ~FilterRegex();
        
    int setRegex( const std::string & _regex );
    
    /**@brief Checks whether a given sentence matches a regular expression
     * @param[in] _sentence The sentence to check
     * @return This function may return
     * SUCCESS on success,
     * DOES_NOT_MATCH if the sentence does not match the regex,
     * INVALID_ARG if the regex is invalid */
    virtual int filter( const sentence & _sentence ) override;
    
    /**@brief Returns true if the regex was accepted */
    bool isRegexValid() const;

private:
    int computeRegex( const std::string & );

private:
    RegexMatcher * m_regex;
    UErrorCode     m_status;
};

// __________________________________________________________________________ //

inline
bool FilterRegex::isRegexValid() const
{
    return !U_FAILURE( m_status );
}


NAMESPACE_END

#endif
