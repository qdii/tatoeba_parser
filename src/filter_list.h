#ifndef FILTER_LIST_H
#define FILTER_LIST_H

#include <tatoparser/listset.h>
#include <tatoparser/sentence.h>
#include <algorithm>
#include "filter.h"

NAMESPACE_START

/**@struct filterList
 * @brief Filters out sentences that are not part of a given list */
struct filterList : public filter
{
    /**@brief Constructs a filterList
     * @param[in] _listName The name of the list */
    filterList( const listset & _listset, const std::string & _listName )
        :m_listset( _listset )
        ,m_hash()
    {
        using namespace qlog;
        std::string lowerCaseName;
        std::transform( _listName.begin(), _listName.end(), std::back_inserter(lowerCaseName), ::tolower );
        assert( lowerCaseName.size() != 0 );
        m_hash = listset::computeHash( lowerCaseName );
        qlog::info << "Computing hash for " << color(yellow) << lowerCaseName << color() << ": " << color( blue ) << m_hash << color() << '\n';
    }

    bool parse( const sentence & _sentence ) TATO_NO_THROW TATO_OVERRIDE
    {
        return m_listset.isSentenceInList( _sentence.getId(), m_hash );
    }

private:
    const listset & m_listset;
    listset::list_hash m_hash;
};
NAMESPACE_END

#endif //FILTER_LIST_H
