#ifndef FILTER_TRANSLATION_REGEX
#define FILTER_TRANSLATION_REGEX

#include <boost/regex.hpp>
#include "filter.h"
#include "dataset.h"


NAMESPACE_START

struct filterTranslationRegex : public filter
{
    filterTranslationRegex( dataset & _dataset, const std::vector<std::string> & _regexList )
        :m_dataset( _dataset )
        ,m_allRegex()
    {
        for( auto regex : _regexList )
            m_allRegex.push_back( boost::make_u32regex( regex ) );
    }

    bool parse( const sentence & _sentence ) __restrict throw() override
    {
        bool ret = false;
        const dataset::linksVector & links = m_dataset.getLinksOf( _sentence.getId() );

        for( auto linkId = links.begin(); !ret && *linkId != sentence::INVALID_ID; ++linkId )
        {
            sentence * link = m_dataset[*linkId];

            if( link )
            {
                try
                {
                    ret = matchAllRegex( *link );
                }
                catch( std::runtime_error & err )
                {
                    qlog::warning << "Cannot match sentence " << *linkId << " with the regex.\n";
                }
            }
        }

        return ret;
    }

    bool matchAllRegex( const sentence & _sentence )
    {
        auto endRegexList = m_allRegex.end();
        bool match = true;

        for( auto regex = m_allRegex.begin(); match && regex != endRegexList; ++regex )
        {
            match &= boost::u32regex_match( _sentence.str(), *regex );
        }

        return match;
    }


private:
    dataset & m_dataset;
    std::vector<boost::u32regex> m_allRegex;
};

NAMESPACE_END

#endif // FILTER_TRANSLATION_REGEX
