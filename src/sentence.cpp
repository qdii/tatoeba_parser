#include "prec_library.h"
#include "tatoparser/sentence.h"
NAMESPACE_START
// -------------------------------------------------------------------------- //

sentence::sentence( sentence::id _id, const char * _lang, const char * _data,
                    const char * _author, const char * _creationDate,
                    const char * _lastModifiedDate )
    :m_id( _id )
    ,m_lang( _lang )
    ,m_data( _data )
    ,m_author( _author )
    ,m_creationDate( _creationDate )
    ,m_lastModifiedDate( _lastModifiedDate )
{
}

// -------------------------------------------------------------------------- //

sentence::sentence( const sentence & _copy )
    :m_id( _copy.m_id )
    ,m_lang( _copy.m_lang )
    ,m_data( _copy.m_data )
    ,m_author( _copy.m_author )
    ,m_creationDate( _copy.m_creationDate )
    ,m_lastModifiedDate( _copy.m_lastModifiedDate )
{
}

// -------------------------------------------------------------------------- //

sentence & sentence::operator=( const sentence & _sentence )
{
    m_id = _sentence.m_id;
    m_lang = _sentence.m_lang;
    m_data = _sentence.m_data;
    m_author = _sentence.m_author;
    m_creationDate = _sentence.m_creationDate;
    m_lastModifiedDate = _sentence.m_lastModifiedDate;

    return *this;
}

// -------------------------------------------------------------------------- //


NAMESPACE_END
