#ifndef FILTER_TAG_H
#define FILTER_TAG_H

#include <string>
#include "dataset.h"

NAMESPACE_START

struct filterTag  : public filter
{
    filterTag( dataset & _data, const std::string _name )
        :m_name( _name )
        ,m_tag( tagset::INVALID_TAGID )
        ,m_data( _data )
    {
    }

    bool parse( const sentence & _sentence ) throw() override
    {
        if( m_tag == tagset::INVALID_TAGID )
            m_tag = m_data.getTagId( m_name );

        return m_data.hasTag( _sentence.getId(), m_tag );
    }

private:
    std::string m_name;
    tagset::tagId m_tag;
    dataset & m_data;
};

NAMESPACE_END

#endif // FILTER_TAG_H
