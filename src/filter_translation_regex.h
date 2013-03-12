#ifndef FILTER_TRANSLATION_REGEX
#define FILTER_TRANSLATION_REGEX

#include <vector>
#include <boost/regex.hpp>
#include <tatoparser/dataset.h>
#include <tatoparser/linkset.h>
#include <tatoparser/sentence.h>
#include "filter.h"
#include "filter_helper_translation.h"

NAMESPACE_START

/**@struct filterTranslationRegex
 * @brief Checks that any of the translations of a sentence matches every regex
 *        in a given set of regular expressions */
struct filterTranslationRegex : public filter, protected filterHelperTranslation
{
    /**@brief Constructs a filterTranslationRegex
     * @param[in] _dataset A container that has information about the sentences
     * @param[in] _linkset A container that knows which links a sentence has
     * @param[in] _regexList Many regular expressions
     * @throw boost::regex_error if any of the regex is invalid */
    filterTranslationRegex( const std::vector<std::string> & _regexList, dataset & _dataset, linkset & _linkset )
        :filterHelperTranslation( _dataset, _linkset )
        ,m_allRegex() // will contain the COMPILED versions of the regular expressions
    {
        // compile the regex and store it for later use
        for( auto regex : _regexList )
            m_allRegex.push_back( boost::make_u32regex( regex ) );
    }

    /**@brief Checks that a sentence has a translation that matches all the regular expressions
     * @param[in] _sentence The sentence to match
     * @return true if one of the sentence translation matches all the regular expressions */
    bool parse( const sentence & TATO_RESTRICT _sentence ) TATO_OVERRIDE
    {
        return doesAnyTranslationRespectCondition<true>( _sentence.getId(),
            [this]( const sentence & _translation ) -> bool
            {
                bool doesTranslationMatch = false;
                try
                {
                    doesTranslationMatch = matchAllRegex( _translation );
                }
                catch( std::runtime_error & )
                {
                    qlog::warning << "An error occurred while matching sentence " << _translation.getId() << " with one of the regex.\n";
                }
                return doesTranslationMatch;
            });
    }

    /**@brief Match a sentence against the set of regular expression
     * @return true if the sentence matches them all
     * @param[in] _sentence The sentence to check */
    bool matchAllRegex( const sentence & _sentence )
    {
        auto endRegexList = m_allRegex.end();
        bool match = true;

        // go through every regex and match the sentence against them
        for( auto regex = m_allRegex.begin(); match && regex != endRegexList; ++regex )
        {
            match &= boost::u32regex_match( _sentence.str(), *regex );
        }

        return match;
    }


private:
    std::vector<boost::u32regex> m_allRegex; // a vector of compiled regex
};

NAMESPACE_END

#endif // FILTER_TRANSLATION_REGEX
