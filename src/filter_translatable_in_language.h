#ifndef FILTER_TRANSLATABLE_IN_LANGUAGE_H
#define FILTER_TRANSLATABLE_IN_LANGUAGE_H

#include <tatoparser/sentence.h>
#include <tatoparser/dataset.h>
#include <tatoparser/linkset.h>
#include <string>
#include "filter.h"

NAMESPACE_START

/**@struct filterTranslatableInLanguage
 * @brief Keep sentences that are translatable in a given language */
struct filterTranslatableInLanguage : public filter, public filterHelperTranslation
{
    /**@brief Constructs a filterTranslatableInLanguage object
     * @param[in] _lang The language in which the sentence should be translatable */
    filterTranslatableInLanguage( const std::string & _lang, dataset & _dataset, linkset & _linkset )
        :filterHelperTranslation( _dataset, _linkset )
        ,m_lang( _lang )
    {
    }

    /**@brief Checks that any of the translation of a sentence is in a given language
     * @param[in] _sentence The sentence to check */
    bool parse( const sentence & TATO_RESTRICT _sentence ) TATO_RESTRICT TATO_NO_THROW TATO_OVERRIDE
    {
        return doesAnyTranslationRespectCondition<true>( _sentence.getId(),
            [this]( const sentence & _translation ) -> bool
            {
                return m_lang == _translation.lang();
            }
        );
    }

private:
    std::string m_lang;    // The language to check for
};

NAMESPACE_END

#endif //FILTER_TRANSLATABLE_IN_LANGUAGE_H
