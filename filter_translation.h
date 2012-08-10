#ifndef FILTER_TRANSLATION_H
#define FILTER_TRANSLATION_H

#include "prec.h"
#include "filter.h"
#include <vector>

NAMESPACE_START

/**@struct FilterTranslation
 * @brief Matches sentences which translation respect some criterions
 */
struct FilterTranslation : public Filter
{
public:
    /**@brief Add a filter
     * @param[in] _filter The filter to add
     * @return SUCCESS
     *
     * This FilterTranslation will filter out sentences which have at least one
     * translation matching the added filters */
    int addFilter( Filter & _filter );

    /**@brief Filters the sentences */
    virtual int filter( const sentence & _sentence ) override;

private:
    std::vector<Filter *> m_filters;
};

NAMESPACE_END
#endif //FILTER_TRANSLATION_H
