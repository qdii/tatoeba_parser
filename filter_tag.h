#ifndef FILTER_TAG_H
#define FILTER_TAG_H

#include "prec.h"
#include "sentence.h"
#include "filter.h"
#include <string>

NAMESPACE_START

struct FilterTag : public Filter
{
    FilterTag( const std::string & _tag );

    /**
     * @brief Filters sentences that are not tagged appropriately
     * @param _sentence The sentence to check
     * @return SUCCESS if the sentences has the tag, DOES_NOT_MATCH otherwise
     */
    virtual int filter( const sentence & _sentence );

private:
    sentence::tag m_tag;
};

NAMESPACE_END

#endif // FILTER_TAG_H
