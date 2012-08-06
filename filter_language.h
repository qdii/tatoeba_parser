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
    FilterLanguage(const std::string & _countryCode);
    FilterLanguage(std::string && _countryCode);
    
    int filter(const sentence &) override;
    
    
private:
    std::string m_countryCode;
};

NAMESPACE_END

#endif // FILTER_LANGUAGE_H
