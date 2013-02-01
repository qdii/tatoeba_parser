#ifndef FILTER_LIST_H
#define FILTER_LIST_H

#include <tatoparser/listset.h>
#include <tatoparser/sentence.h>
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
        ,m_hash( listset::computeHash( _listName ) )
    {
    }

    bool parse( const sentence & _sentence ) TATO_NO_THROW TATO_OVERRIDE
    {
        return m_listset.isSentenceInList( _sentence.getId(), m_hash );
    }

private:
    listset::list_hash m_hash;
    const listset & m_listset;
};
NAMESPACE_END

#endif //FILTER_LIST_H
