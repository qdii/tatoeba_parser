#ifndef FILTER_ID_H
#define FILTER_ID_H

#include "filter.h"
#include "sentence.h"

NAMESPACE_START

/**@struct filterId
 * @brief Checks that a sentence has a given id */
struct filterId : public filter
{
    /**@brief Constructs a filterId
     * @param[in] The id to check the sentence against */
    filterId( sentence::id _id ) TATO_NO_THROW
        :m_filteredId( _id )
    {
    }

    /**@brief Checks that a sentence has a given id
     * @param[in] _sentence The sentence to check
     * @return true if it does, false otherwise */
    virtual bool parse( const sentence & _sentence ) TATO_NO_THROW TATO_OVERRIDE
    {
        return _sentence.getId() == m_filteredId;
    }

private:
    sentence::id m_filteredId;
};

NAMESPACE_END

#endif // FILTER_H
