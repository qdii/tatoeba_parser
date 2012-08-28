#ifndef FILTER_LANGUAGE_H
#define FILTER_LANGUAGE_H

#include "prec.h"
#include "filter.h"
#include <string>

NAMESPACE_START

struct sentence;

/**@struct FilterLanguage
 * @brief Checks whether a sentence is in a given language */
struct FilterLanguage : public Filter
{
    int setCountryCode( const std::string & _countryCode );
    int filter( const sentence & ) override;

private:
    std::string m_countryCode;
};

// -------------------------------------------------------------------------- //

inline
int FilterLanguage::setCountryCode( const std::string & _countryCode )
{
    m_countryCode.assign( _countryCode );
    return SUCCESS;
}

NAMESPACE_END

#endif // FILTER_LANGUAGE_H
