#ifndef FILTER_TRANSLATION_REGEX
#define FILTER_TRANSLATION_REGEX

#include <boost/regex.hpp>
#include "filter.h"
#include "dataset.h"


NAMESPACE_START

struct filterTranslationRegex : public filter
{
    filterTranslationRegex( dataset & _dataset, const std::string & _regex )
        :m_dataset( _dataset )
        ,m_regex( boost::make_u32regex( _regex ) )
    {
    }

    bool parse( const sentence & _sentence ) __restrict throw() override
    {
        bool ret = false;
        const std::vector<sentence::id> & links = m_dataset.getLinksOf( _sentence.getId() );

        for( auto linkId = links.begin(); !ret && linkId != links.end(); ++linkId )
        {
            sentence * link = m_dataset[*linkId];

            if( link )
            {
                try
                {
                    ret = boost::u32regex_match( link->str(), m_regex );
                }
                catch( std::runtime_error & err )
                {
                    qlog::warning << "Cannot match sentence " << *linkId << " with the regex.\n";
                }
            }
        }

        return ret;
    }

private:
    dataset & m_dataset;
    boost::u32regex m_regex;
};

NAMESPACE_END

#endif // FILTER_TRANSLATION_REGEX
