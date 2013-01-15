#ifndef TATO_PARSER_SENTENCE_H
#define TATO_PARSER_SENTENCE_H

#include <cstdint>
#include "namespace.h"

NAMESPACE_START

struct sentence
{
    /**@brief A number that identifies the sentence uniquely */
    typedef uint32_t id;
    static const uint32_t INVALID_ID = 0;

    /**@brief Constructs a sentence
     * @param[in] _id   An unique identifier for the sentence
     * @param[in] _lang The letters of the country
     * @param[in] _data The text that coposes the sentence
     * @param[in] _author The nickname of the creator of the sentence
     * @param[in] _creationDate When the sentence was first entered
     * @param[in] _lastModifiedDate When the sentence was last modified */
    explicit sentence(
        sentence::id _id   = INVALID_ID,
        const char * _lang = nullptr,
        const char * _data = nullptr,
        const char * _author = nullptr,
        const char * _creationDate = nullptr,
        const char * _lastModifiedDate = nullptr
    );

    /**@brief Destructs a sentence */
    ~sentence() { }

    /**@brief Construct a sentence by copy
     * @param[in] _copy A sentence to copy */
    sentence( const sentence & _sentence );

    /**@brief Assigns a sentence to another
     * @param[in] _other The other sentence to assign to this one
     * @return *this */
    sentence & operator=( const sentence & _sentence );

    /**@brief Returns an unique identifier identifying the sentence
     * @return A number that identify uniquely this sentence */
    id getId() const;

    /**@brief Returns a pointer to the data */
    const char * str() const { return m_data; }

    /**@brief Returns a pointer to a character string representing the language */
    const char * lang() const { return m_lang; }

    /**@brief Returns a pointer to the author nickname */
    bool belongsTo( const std::string & _user ) const { return _user == m_author; }

private:
    id           m_id;
    const char * m_lang;
    const char * m_data;
    const char * m_author;
    const char * m_creationDate;
    const char * m_lastModifiedDate;
};

// -------------------------------------------------------------------------- //

inline sentence::id sentence::getId() const
{
    return m_id;
}



NAMESPACE_END
#endif  // TATO_PARSER_SENTENCE_H
