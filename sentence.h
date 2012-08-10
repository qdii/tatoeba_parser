#ifndef SENTENCE_H
#define SENTENCE_H

#include "prec.h"
#include <string>
#include <iostream>
#include <assert.h>
#include <unistr.h>
#include <umachine.h>
#include <vector>

NAMESPACE_START

/**@brief A sentence
 * @desc A sentence, according to tatoeba has not only text but also a country code */
struct sentence
{
    typedef unsigned long id;

    sentence( id _id, std::string &	_text, char _country[5] );
    sentence( id _id, std::string && _text, char _country[5] );

    /**@brief Returns the language
       @desc a 3 to 5-character code that identifies the country where the language is spoken */
    const char * getCountryCode() const;

    /**@brief Returns the text of the sentence */
    const UnicodeString & text() const;

    /**@brief Returns the UTF-16, raw, text of the sentence */
    const UChar * textRaw() const;

    /**@brief Returns an unique identifier for the sentence */
    id  getId() const;

    friend std::ostream & operator <<( std::ostream &, const sentence & );
    
public:
    /**@brief link a sentence to this one
     * @param[in] _translation The sentence to link to this one
     * @return SUCCESS on succes, INVALID_ARG if the sentence linked is itself */
    int addLink(sentence & _translation);
    
    /**@brief checks if a sentence is linked to this one 
     * @param[in] _candidate the sentence that may be linked to this one */
    bool isLinked(const sentence & _candidate) const;
    
    std::vector<sentence *> & getAllTranslations();
    const std::vector<sentence *> & getAllTranslations() const;
private:
    id			    m_id;
    UnicodeString   m_text;
    std::string	    m_country;
    std::vector<sentence*> m_links;
};

// __________________________________________________________________________ //

inline
const char * sentence::getCountryCode() const
{
    return m_country.c_str();
}

// __________________________________________________________________________ //

inline
const UnicodeString & sentence::text() const
{
    return m_text;
}

// __________________________________________________________________________ //

inline
const UChar * sentence::textRaw() const
{
    return const_cast<UnicodeString &>( text() ).getTerminatedBuffer();
}

// __________________________________________________________________________ //

inline
sentence::id  sentence::getId() const
{
    return m_id;
}

// __________________________________________________________________________ //

inline
std::vector<sentence *> & sentence::getAllTranslations()
{
    return m_links;
}

// __________________________________________________________________________ //

inline
const std::vector<sentence *> & sentence::getAllTranslations() const
{
    return m_links;
}

NAMESPACE_END

#endif //SENTENCE_H
