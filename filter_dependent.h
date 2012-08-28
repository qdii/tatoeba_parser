#ifndef FILTER_DEPENDENT_H
#define FILTER_DEPENDENT_H

#include "prec.h"
#include "filter.h"
#include <vector>

NAMESPACE_START

/**@struct FilterDependent
 * @brief Filter a sentence only if all the internal filters match it */
struct FilterDependent : public Filter
{
    /**@brief Adds a filter
     * @param[in] _filter The filter to add 
     * @return SUCCESS. */
    int addFilter(Filter & _filter);

    int filter( const sentence & _sentence ) override;
    
private:
    typedef std::vector<Filter*> FilterList;
    FilterList m_filterList;
};

NAMESPACE_END

#endif // FILTER_DEPENDENT_H
