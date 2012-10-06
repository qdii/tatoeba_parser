#ifndef FILTER_TRANSLATABLE_IN_LANGUAGE_H
#define FILTER_TRANSLATABLE_IN_LANGUAGE_H

#include "filter.h"

NAMESPACE_START

/**@struct filterTranslatableInLanguage
 * @brief Keep sentences that are translatable in a given language */
struct filterTranslatableInLanguage : public filter
{
    /**@brief Constructs a filterTranslatableInLanguage object
     * @param[in] _dataset Contains the sentences
     * @param[in] _linkset Contains the data about links
     * @param[in] _lang The language in which the sentence should be translatable */
    filterTranslatableInLanguage( dataset & _dataset, linkset & _linkset, const std::string & _lang )
        :m_dataset( _dataset )
        ,m_linkset( _linkset )
        ,m_lang( _lang )
    {
    }

    /**@brief Checks that any of the translation of a sentence is in a given language
     * @param[in] _sentence The sentence to check */
    bool parse( const sentence & TATO_RESTRICT _sentence ) TATO_RESTRICT TATO_NO_THROW TATO_OVERRIDE
    {
        // get the ids of the translations of this sentence
        const sentence::id * TATO_RESTRICT allLinksOfSentence =
            m_linkset.getLinksOf( _sentence.getId() );

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
    dataset  &  m_dataset; // contains info about the sentence (namely, its language)
    linkset  &  m_linkset; // contains info about the translations of a sentenc
    std::string m_lang;    // The language to check for
};

NAMESPACE_END

#endif //FILTER_TRANSLATABLE_IN_LANGUAGE_H
