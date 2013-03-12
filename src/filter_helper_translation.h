#ifndef FILTER_HELPER_TRANSLATION
#define FILTER_HELPER_TRANSLATION

#include <tatoparser/linkset.h>

NAMESPACE_START

/**@struct filterHelperTranslation
 * @brief Provides helper functions for filter translations */
struct filterHelperTranslation
{
    filterHelperTranslation( dataset & _dataset, linkset & _linkset )
        :m_linkset( _linkset )
        ,m_dataset( _dataset )
    {
    }


    /**@brief Checks that all the translation of a sentence match a condition
     * @param[in] _originalSentence The sentence which translations should be matched
     * @param[in] _condition A function taking a sentence as parameter and returning a bool to
     *            use as a condition.
     * @tparam CONDITION is a signature of a function of the form bool foo(const sentence &);
     * @tparam STRICT sets the behavior of the function when the sentence has no translation. If
     *         STRICT is set to true, then the function returns false, otherwise it returns true.
     * @return true if all the translations of the sentence match. */
    template<bool STRICT, typename CONDITION>
    bool doAllTranslationRespectCondition( sentence::id _originalSentence, CONDITION _condition )
    {
        // get the ids of the translations of this sentence
        auto allLinksOfSentence = m_linkset.getLinksOfSafe( _originalSentence );

        bool allTranslationAreRespectful = allLinksOfSentence.first == allLinksOfSentence.second ? true : !STRICT;

        // go through the translations and check if any of them matches the language the user set
        for( auto iter = allLinksOfSentence.first; iter != allLinksOfSentence.second && allTranslationAreRespectful ; ++iter )
        {
            // sometimes links.csv references sentences that just don’t exist in sentences.csv
            if( m_dataset[*iter] != nullptr )
            {
                assert( m_dataset[*iter]->lang() );

                allTranslationAreRespectful &= _condition( *m_dataset[*iter] );
            }
        }

        return allTranslationAreRespectful;
    }

    /**@brief Checks that any of the translation of a sentence match a condition
     * @param[in] _originalSentence The sentence which translations should be checked
     * @param[in] _condition A function taking a sentence as parameter and returning a bool to
     *            use as a condition.
     * @tparam CONDITION is a signature of a function of the form bool foo(const sentence &);
     * @tparam STRICT sets the behavior of the function when the sentence has no translation. If
     *         STRICT is set to true, then the function returns false, otherwise it returns true.
     * @return true if all the translations of the sentence match. */
    template<bool STRICT, typename CONDITION>
    bool doesAnyTranslationRespectCondition( sentence::id _originalSentence, CONDITION _condition )
    {
        // get the ids of the translations of this sentence
        auto allLinksOfSentence = m_linkset.getLinksOfSafe( _originalSentence );

        bool atLeastOneTranslationIsRespectful = allLinksOfSentence.first == allLinksOfSentence.second ? false : !STRICT;

        // go through the translations and check if any of them matches the language the user set
        for( auto iter = allLinksOfSentence.first; iter != allLinksOfSentence.second && !atLeastOneTranslationIsRespectful; ++iter )
        {
            // sometimes links.csv references sentences that just don’t exist in sentences.csv
            if( m_dataset[*iter] != nullptr )
            {
                assert( m_dataset[*iter]->lang() );

                atLeastOneTranslationIsRespectful |= _condition( *m_dataset[*iter] );
            }
        }

        return atLeastOneTranslationIsRespectful;
    }

private:
    linkset & m_linkset;
    dataset & m_dataset;
};

NAMESPACE_END

#endif //FILTER_HELPER_TRANSLATION
