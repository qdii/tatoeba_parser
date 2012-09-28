#ifndef TATO_PARSER_SENTENCE_H
#define TATO_PARSER_SENTENCE_H

#include <cstdint>

NAMESPACE_START

struct sentence
{
    /**@brief A number that identifies the sentence uniquely */
    typedef int32_t id;
    static const id INVALID_ID = 0;

    /**@brief Constructs a sentence
     * @param[in] _id   An unique identifier for the sentence
     * @param[in] _lang The letters of the country
     * @param[in] _data The text that coposes the sentence */
    explicit sentence(
        sentence::id _id   = INVALID_ID ,
        const char * _lang = nullptr ,
        const char * _data = nullptr
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

private:
    id           m_id;
    const char * m_lang;
    const char * m_data;
} __attribute__((packed)) ;

// -------------------------------------------------------------------------- //

inline sentence::id sentence::getId() const
{
    return m_id;
}



NAMESPACE_END
#endif  // TATO_PARSER_SENTENCE_H
