#ifndef FILTER_ID_H
#define FILTER_ID_H

#include "prec.h"
#include "sentence.h"

NAMESPACE_START

/**@struct FilterId
 * @brief Filters sentences that do not match the specified id */
struct FilterId : public Filter
{
    /**@brief Constructs a FilterId 
     * @param[in] _id The id of the sentence you want to filter in */
    FilterId(sentence::id _id) : m_id(_id) { }

    int filter( const sentence & _sentence ) override;
    
private:
    sentence::id m_id;
};

// -------------------------------------------------------------------------- //

inline
int FilterId::filter( const sentence & _sentence )
{
    return _sentence.getId() == m_id ? SUCCESS : DOES_NOT_MATCH;
}

NAMESPACE_END

#endif // FILTER_ID_H
