#ifndef FILTER_TAG_H
#define FILTER_TAG_H

#include <string>
#include <tatoparser/tagset.h>
#include <tatoparser/sentence.h>
#include "filter.h"

NAMESPACE_START

/**@struct filterTag
 * @brief Checks whether a sentence has a particular tag */
struct filterTag  : public filter
{
    /**@brief Constructs a filterTag object
     * @param[in] _allTags A container that stores all the tags
     * @param[in] _name The name of the tag to check for */
    filterTag( const std::string & _name, tagset & _allTags )
        :m_name( _name )
        ,m_tag( tagset::INVALID_TAGID )
        ,m_allTags( _allTags )
    {
    }

    /**@brief Checks that a sentence has a given tag
     * @param[in] _sentence The sentence to check       */
    bool parse( const sentence & _sentence ) TATO_NO_THROW TATO_OVERRIDE
    {
        // if we didn’t check the tag id yet, we do it now
        if( TATO_UNLIKELY( m_tag == tagset::INVALID_TAGID ) )
            m_tag = m_allTags.getTagId( toLower( m_name ) );

        return m_allTags.isSentenceTagged( _sentence.getId(), m_tag );
    }
private:
    std::string m_name;  // the name of the tag
    tagset::tagId m_tag; // the id of the tag which name is m_name
    tagset & m_allTags;  // this tells us the tags of a sentence
};

NAMESPACE_END

#endif // FILTER_TAG_H
