#ifndef FILTER_TRANSLATION_REGEX
#define FILTER_TRANSLATION_REGEX

#include <boost/regex.hpp>
#include "filter.h"
#include "dataset.h"
#include "linkset.h"


NAMESPACE_START

/**@struct filterTranslationRegex
 * @brief Checks that any of the translations of a sentence matches every regex
 *        in a given set of regular expressions */
struct filterTranslationRegex : public filter
{
    /**@brief Constructs a filterTranslationRegex
     * @param[in] _dataset A container that has information about the sentences
     * @param[in] _linkset A container that knows which links a sentence has
     * @param[in] _regexList Many regular expressions
     * @throw boost::regex_error if any of the regex is invalid */
    filterTranslationRegex( dataset & _dataset, linkset & _linkset, const std::vector<std::string> & _regexList )
        :m_dataset( _dataset )
        ,m_linkset( _linkset )
        ,m_allRegex() // will contain the COMPILED versions of the regular expressions
    {
        // compile the regex and store it for later use
        for( auto regex : _regexList )
            m_allRegex.push_back( boost::make_u32regex( regex ) );
    }

    /**@brief Checks that a sentence has a translation that matches all the regular expressions
     * @param[in] _sentence The sentence to match
     * @return true if one of the sentence translation matches all the regular expressions */
    bool parse( const sentence & __restrict _sentence ) TATO_OVERRIDE __restrict TATO_NO_THROW
    {
        bool ret = false;

        // links is a NULL terminated list of sentence::id, each of them being a
        // translation of the passed sentence
        const sentence::id * __restrict links =
            m_linkset.getLinksOf( _sentence.getId() );

        // match every translation against the set of regular expressions
        for ( sentence::id linkId = *links; linkId != sentence::INVALID_ID; linkId = *++links)
        {
            // the i-th translation
            sentence * __restrict link = m_dataset[linkId];

            if( link ) // the sentence might not exist if sentences.csv and links.csv are inconsistent
            {
                try
                {
                    ret = matchAllRegex( *link );
                }
                catch( std::runtime_error & err )
                {
                    qlog::warning << "An error occurred while matching sentence " << linkId << " with one of the regex.\n";
                }
            }
        }

        return ret;
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
    dataset & m_dataset; // gives the text of the sentences to match against the regex
    linkset & m_linkset; // contains info about the translations of the sentence
    std::vector<boost::u32regex> m_allRegex; // a vector of compiled regex
};

NAMESPACE_END

#endif // FILTER_TRANSLATION_REGEX
