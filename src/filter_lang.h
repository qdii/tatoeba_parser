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
    filterLang( const std::string & _lang ): m_lang( _lang ) { }

    /**@brief Checks that a sentence is in the right language
     * @param[in] _sentence The sentence to check against the language
     * @return true if the sentence is in the right language */
    virtual bool parse( const sentence & _sentence ) TATO_OVERRIDE
    {
        return m_lang == _sentence.lang();
    }

private:
    std::string m_lang;
};

NAMESPACE_END

#endif // FILTER_LANG_H
