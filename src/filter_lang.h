#ifndef FILTER_LANG_H
#define FILTER_LANG_H

#include "filter.h"
#include "sentence.h"

NAMESPACE_START

/**@struct filterLang
 * @brief Checks whether a sentence is in the right language */
struct filterLang : public filter
{
    /**@brief Constructs a filterLang
     * @param[in] _lang A language the sentence will be checked against */
    filterLang( const std::vector<std::string> & _lang )
        : m_languages( _lang )
    {
    }

    /**@brief Checks that a sentence is in the right language
     * @param[in] _sentence The sentence to check against the language
     * @return true if the sentence is in the right language */
    virtual bool parse( const sentence & _sentence ) TATO_OVERRIDE
    {
        for(
            std::vector<std::string>::const_iterator matchLanguage = m_languages.begin();
            matchLanguage != m_languages.end(); ++matchLanguage )
        {
            if( *matchLanguage == _sentence.lang() )
                return true;
        }

        return false;
    }

private:
    std::vector<std::string> m_languages;
};

NAMESPACE_END

#endif // FILTER_LANG_H
