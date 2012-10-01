#ifndef FILTER_TRANSLATABLE_IN_LANGUAGE_H
#define FILTER_TRANSLATABLE_IN_LANGUAGE_H

#include "filter.h"

NAMESPACE_START

/**@struct filterTranslatableInLanguage
 * @brief Keep sentences that are translatable in a given language */
struct filterTranslatableInLanguage : public filter
{
    filterTranslatableInLanguage( dataset & _dataset, const std::string & _lang )
        :m_dataset( _dataset )
        ,m_lang( _lang )
    {
    }

    bool parse( const sentence & __restrict _sentence ) __restrict throw() TATO_OVERRIDE
    {
        // get the ids of the translations of this sentence
        const sentence::id * __restrict allLinksOfSentence =
            m_dataset.getLinksOf( _sentence.getId() );

        if( allLinksOfSentence )
        {
            // go through the translations and check if any of them matches the language the user set
            while( *allLinksOfSentence != sentence::INVALID_ID )
            {
                // sometimes links.csv references sentences that just don’t exist
                if( m_dataset[*allLinksOfSentence] != nullptr )
                {
                    assert( m_dataset[*allLinksOfSentence]->lang() );

                    if( m_lang == m_dataset[*allLinksOfSentence]->lang() )
                        return true;
                }

                ++allLinksOfSentence;
            }
        }

        return false;
    }


private:
    dataset  &  m_dataset;
    std::string m_lang;
};

NAMESPACE_END

#endif //FILTER_TRANSLATABLE_IN_LANGUAGE_H
