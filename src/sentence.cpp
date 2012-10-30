#include "prec_library.h"
#include "tatoparser/sentence.h"
NAMESPACE_START
// -------------------------------------------------------------------------- //

sentence::sentence( sentence::id _id, const char * _lang, const char * _data )
    :m_id( _id )
    ,m_lang( _lang )
    ,m_data( _data )
{
}

// -------------------------------------------------------------------------- //

sentence::sentence( const sentence & _copy )
    :m_id( _copy.m_id )
    ,m_lang( _copy.m_lang )
    ,m_data( _copy.m_data )
{
}

// -------------------------------------------------------------------------- //

sentence & sentence::operator=( const sentence & _sentence )
{
    m_id = _sentence.m_id;
    m_lang = _sentence.m_lang;
    m_data = _sentence.m_data;

    return *this;
}

// -------------------------------------------------------------------------- //


NAMESPACE_END
