#ifndef FILTER_LINK_H
#define FILTER_LINK_H

#include "filter.h"
#include "dataset.h"

/**@struct filterLink
 * @brief Checks that a sentence is linked to another */
struct filterLink : public filter
{
    filterLink(dataset & _dataset, sentence::id _id)
        :m_dataset(_dataset)
        ,m_id(_id)
    {
    }
    
    bool parse(const sentence & _sentence) throw () override
    {
        return m_dataset.areLinked( _sentence.getId(), m_id);
    }
private:
    dataset & m_dataset;
    sentence::id m_id;
};
#endif // FILTER_LINK_H
