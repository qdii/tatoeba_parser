#include "prec.h"
#include "selecter.h"
#include <assert.h>
#include <string.h> //memcmp
#include <algorithm> // remove_if, find

NAMESPACE_START

// __________________________________________________________________________ //

selecter::selecter()
    :m_allowedCharacters( nullptr )
    ,m_regularExpression( nullptr )
{
    m_hasCountryCode[0] = 0;
}
// __________________________________________________________________________ //

selecter::~selecter()
{
    delete m_regularExpression;
    delete m_allowedCharacters;
}

// __________________________________________________________________________ //

int selecter::canContainCharacters( const char * _characters )
{
    int ret = SUCCESS;

    if( _characters )
    {
        if( !m_allowedCharacters )
            m_allowedCharacters = new UnicodeString;

        if( m_allowedCharacters )
            *m_allowedCharacters = _characters;
        else
            ret = INVALID_ARG;
    }
    else
    {
        delete m_allowedCharacters;
        m_allowedCharacters = nullptr;
    }

    return ret;
}

// __________________________________________________________________________ //

int selecter::hasCountryCode( const char _countryCode [5] )
{
    memcpy( m_hasCountryCode, _countryCode, 5 );
    return SUCCESS;
}

// __________________________________________________________________________ //

int selecter::matches( const sentence & _candidate )
{
    int ret = SUCCESS;

    // checking country code
    if( m_hasCountryCode[0] && strncmp( _candidate.getCountryCode(), m_hasCountryCode, 5 ) != 0 )
        ret = DOES_NOT_MATCH;

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
            ret = DOES_NOT_MATCH;
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
                ret = DOES_NOT_MATCH;

            isAllowed = false;
        }
    }

    // checking regular expression
    if( m_regularExpression && ret == 0 )
    {
        m_regularExpression->reset( _candidate.text() );

        if( !m_regularExpression->find() )
            ret = DOES_NOT_MATCH;
    }

    return ret;
}

// __________________________________________________________________________ //

int selecter::setMustContainCharacters( const char * _characters )
{
    m_compulsoryCharacters = _characters;
    return SUCCESS;
}

// __________________________________________________________________________ //

int selecter::matchRegularExpression( const std::string & _regex )
{
    int ret = SUCCESS;

    if( !m_regularExpression )
    {
        UErrorCode     status   = U_ZERO_ERROR;
        m_regularExpression     = new RegexMatcher( _regex.c_str(), 0, status );

        if( m_regularExpression && U_FAILURE( status ) )
            ret = INVALID_ARG;
    }

    if( !m_regularExpression && ret == SUCCESS )
        ret = OUT_OF_MEMORY;

    return ret;
}

NAMESPACE_END
