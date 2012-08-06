#ifndef SELECTER_H
#define SELECTER_H

#include "prec.h"
#include "sentence.h"
#include <vector>

NAMESPACE_START

struct Filter;

/**@struct selecter
 * @brief Tells if a sentence match a set of filters.
 *
 * Users should basically set their criterions using the different member
 * functions, then run matches(), passing it the sentence to check. */
struct selecter
{
    /**@brief Add a filter to the list
     * @param[in] _filter The filter to add
     * @return SUCCESS */
    int addFilter( Filter & _filter );

    /**@brief Removes all the filters
     * @return SUCCESS */
    int clear();

    /**@brief The string to check
     * @return SUCCESS if the string matches, DOES_NOT_MATCH if it does not, INTERNAL_ERROR on error */
    int matches( const sentence & _stringToCheck );

private:
    typedef std::vector<Filter *> Filters;
    Filters m_filters;
};


NAMESPACE_END

// __________________________________________________________________________ //

inline
int selecter::addFilter( Filter & _filter )
{
    m_filters.push_back( &_filter );
    return SUCCESS; 
}

// __________________________________________________________________________ //

inline
int selecter::clear()
{
    m_filters.clear();
    return SUCCESS;
}


#endif //SELECTER_H
