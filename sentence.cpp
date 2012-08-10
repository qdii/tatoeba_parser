#include "prec.h"
#include "sentence.h"
#include <string.h>
#include <algorithm>

NAMESPACE_START

sentence::sentence( id _id, std::string & _text, char _country[5] )
    :m_id( _id )
    ,m_text( UnicodeString::fromUTF8( _text ) )
{
    ASSERT( _country[3] == '\0' || _country[4] == '\0' );
    m_country.reserve( 5 );
    m_country.assign( _country );
}

// __________________________________________________________________________ //

sentence::sentence( id _id, std::string && _text, char _country[5] )
    :m_id( _id )
    ,m_text( UnicodeString::fromUTF8( _text ) )
{
    ASSERT( _country[3] == '\0' || _country[4] == '\0' );
    m_country.reserve( 5 );
    m_country.assign( _country );
}

// __________________________________________________________________________ //

std::ostream & operator << ( std::ostream & _ostr, const sentence & _sentence )
{
    // here we will convert the  UTF-16 encoded text into some printable UTF-8
    const UnicodeString & utf16text = _sentence.text();

    // calling extract function with null output to retrieve the necessary size
    const int32_t bufferSize =
        utf16text.extract( 0, utf16text.length(), nullptr, nullptr ) + 1; // NULL character

    char * const utf8output = new char[bufferSize];

    if( utf8output )
    {
        utf16text.extract( 0, utf16text.length(), utf8output, nullptr );
        utf8output[bufferSize-1] = '\0';
        _ostr << utf8output;
        delete [] utf8output;
    }

    return _ostr;
}

// __________________________________________________________________________ //

int sentence::addLink(sentence& _translation)
{
    m_links.push_back(&_translation);
    
    /* Here we could decide not to trust links.csv and check whether the links
       are unique. I am both gullible and lazy. */
    
    return SUCCESS;
}

// __________________________________________________________________________ //

bool sentence::isLinked(const sentence& _candidate) const
{
    const sentence::id candidateId = _candidate.getId();
    return std::find_if(m_links.begin(), m_links.end(), 
        [=](sentence * _sentence)
        {
            return _sentence->getId() == candidateId;
        }) != m_links.end();
}


NAMESPACE_END

