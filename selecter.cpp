#include "selecter.h"
#include <assert.h>
#include <string.h> //memcmp
#include <algorithm> // remove_if, find
selecter::selecter()
    :m_allowedCharacters( nullptr )
{
    m_hasCountryCode[0] = 0;
}

selecter::~selecter()
{
    delete m_allowedCharacters;
}

int selecter::canContainCharacters( const char * _characters )
{
    int ret = 0;

    if( _characters )
    {
        if( !m_allowedCharacters )
            m_allowedCharacters = new UnicodeString;

        if( m_allowedCharacters )
            *m_allowedCharacters = _characters;
        else
            ret = -1;
    }
    else
    {
        delete m_allowedCharacters;
        m_allowedCharacters = nullptr;
    }

    return ret;
}

int selecter::hasCountryCode( const char _countryCode [5] )
{
    memcpy( m_hasCountryCode, _countryCode, 5 );
    return 0;
}

int selecter::matches( const sentence & _candidate )
{
    int ret = 0;

    // checking country code
    if( m_hasCountryCode[0] && strncmp( _candidate.getCountryCode(), m_hasCountryCode, 5 ) != 0 )
        ret = -1;

    // checking compulsory character set (faster process if we sort the lists before)
    const UnicodeString & text = _candidate.text();

    for( int i = 0; ret == 0 && i< m_compulsoryCharacters.countChar32(); ++i )
    {
        const UChar character = m_compulsoryCharacters.charAt( i );

        // is it contained in our candidate?
        bool isContained = false;

        for( int j = 0; !isContained && j < text.countChar32(); ++j )
        {
            isContained |= ( character == text.charAt( j ) );
        }

        if( !isContained )
            ret = -1;
    }

    // checking allowed character set
    if( m_allowedCharacters && ret == 0 )
    {
        bool isAllowed = false;
        const int32_t nbAllowedCharacters = m_allowedCharacters->countChar32();
        UChar currentChar, allowedChar;

        for( int textIter = 0; ret == 0 && textIter < text.countChar32(); ++textIter )
        {
            // does the i-th character belong to the list of allowed char?
            currentChar = text.charAt( textIter );

            for( int allowIter = 0; !isAllowed && allowIter < nbAllowedCharacters; ++allowIter )
            {
                allowedChar = m_allowedCharacters->charAt( allowIter );

                if( allowedChar == currentChar )
                    isAllowed = true;
            }

            if( !isAllowed )
                ret = -1;

            isAllowed = false;
        }
    }

    return ret;
}

int selecter::setMustContainCharacters( const char * _characters )
{
    m_compulsoryCharacters = _characters;
    return 0;
}
