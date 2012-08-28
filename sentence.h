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
    typedef std::hash<std::string>::result_type tag;
    static const id INVALID_ID = static_cast<id>(0);

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
    int addLink( sentence & _translation );

    /**@brief checks if a sentence is linked to this one
     * @param[in] _candidate the sentence that may be linked to this one */
    bool isLinked( const sentence & _candidate ) const;

    std::vector<sentence *> & getAllTranslations();
    const std::vector<sentence *> & getAllTranslations() const;

public:
    /**@brief Add a tag to this sentence
     * @param[in] _tagName the string of the tag, for instance "less than 8 words"
     * @return SUCCESS on success */
    int addTag( const std::string & _tagName );
    
    /**@brief Add a tag to this sentence
     * @param[in] _tag the tag"
     * @return SUCCESS on success */
    int addTag( sentence::tag _tag );

    /**@brief Checks if the sentence has a given tag
     * @return SUCCESS if it does, NO_SUCH_TAG if it does not */
    int hasTag( const std::string & _tagName ) const;
    
    /**@brief Checks if the sentence has a given tag
     * @return SUCCESS if it does, NO_SUCH_TAG if it does not */
    int hasTag( sentence::tag _tag ) const;

    static tag  getHashFromName( const std::string & _hashName );
    
private:
    id			    m_id;
    UnicodeString   m_text;
    std::string	    m_country;
    std::vector<sentence *> m_links;
    std::vector<tag> m_tags;

    static std::hash<std::string> m_hasher;

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

// __________________________________________________________________________ //

inline
sentence::tag  sentence::getHashFromName( const std::string & _hashName )
{
    return m_hasher( _hashName );
}

// __________________________________________________________________________ //

inline
int sentence::hasTag(const std::string & _tagName) const
{
    return hasTag( sentence::getHashFromName(_tagName));
}


NAMESPACE_END

#endif //SENTENCE_H
