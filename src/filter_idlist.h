#ifndef FILTER_ID_LIST_H
#define FILTER_ID_LIST_H

#include "filter.h"
#include <tatoparser/sentence.h>
#include <vector>

NAMESPACE_START

/**@struct filterIdList
 * @brief Checks that a sentence has any of the given ids */
struct filterIdList : public filter
{
    /**@brief Constructs a filterId
     * @param[in] The id to check the sentence against */
    template<typename ID_VECTOR>
    filterIdList( ID_VECTOR && _ids ) TATO_NO_THROW
        :m_filteredIds( std::move(_ids) )
    {
    }

    /**@brief Checks that a sentence has a given id
     * @param[in] _sentence The sentence to check
     * @return true if it does, false otherwise */
    virtual bool parse( const sentence & _sentence ) TATO_NO_THROW TATO_OVERRIDE
    {
        return std::find( m_filteredIds.begin(), m_filteredIds.end(), _sentence.getId() ) != m_filteredIds.end();
    }

private:
    std::vector<sentence::id> m_filteredIds;
};

NAMESPACE_END

#endif // FILTER_ID_LIST_H
