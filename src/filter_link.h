#ifndef FILTER_LINK_H
#define FILTER_LINK_H

#include "filter.h"
#include <tatoparser/linkset.h>
#include <tatoparser/sentence.h>

NAMESPACE_START

/**@struct filterLink
 * @brief Checks that a sentence is linked to another one. */
struct filterLink : public filter
{
    /**@brief Constructs a filterLink
     * @param[in] _linkset A list of the links of the sentences
     * @param[in] _id The id of the first sentence. */
    filterLink( sentence::id _id, linkset & _linkset )
        :m_linkset( _linkset )
        ,m_id( _id )
    {
    }

    /**@brief Checks that a sentence is linked to another one
     * @param[in] _sentence The second sentence.
     * @return True if the passed sentence is linked to the sentence passed in
     * the filterLink constructor */
    bool parse( const sentence & _sentence ) throw() TATO_OVERRIDE
    {
        return m_linkset.areLinked( _sentence.getId(), m_id );
    }

private:
    linkset & m_linkset;
    sentence::id m_id;
};

NAMESPACE_END

#endif // FILTER_LINK_H
