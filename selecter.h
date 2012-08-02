#ifndef SELECTER_H
#define SELECTER_H

#include "sentence.h"
#include <string>


/**@struct selecter
 * @brief Tells if a sentence match a set of criterions.
 * 
 * Users should basically set their criterions using the different member
 * functions, then run matches(), passing it the right sentence. */
struct selecter
{
    /**@brief Construct a selecter */
    selecter();
    
    /**@brief Destructs a selecter */
    ~selecter();
    
    /**@brief Set the list of characters which must be present in the sentence 
     * 
     * If a sentence does contain none of the characters passed here, then
     * the matches() member function will return false when called on it.
     * 
     * @return 0 on success, -1 if _characters is null.
     * @param[in] _characters A null-terminated set of characters that have to
     *            be present in the sentence.
     * @pre _characters != nullptr */
    int setMustContainCharacters(const char * _characters);
    
    /**@brief Set the list of characters which must be present in the sentence 
     * 
     * If a sentence does contain none of the characters passed here, then
     * the matches() member function will return false when called on it.
     * 
     * @return 0 on success, -1 if _characters is null.
     * @param[in] _characters A string containing the characters that have to
     *            be present in the sentence.
     * @pre _characters != nullptr */
    int setMustContainCharacters(const std::string & _characters);
    
    /**@brief Set the list of characters that can appear in the sentence 
     * 
     * Determines which characters are allowed to appear in the sentence.
     * @param[in] _characters If nullptr is passed, any characters are allowed.
     *            if a null-terminated char array is passed, a sentence that 
     *            contains any other character will be discarded.
     * 
     * */
    int canContainCharacters(const char * _characters);
    
    /**@brief Checks whether the sentence matches the country code 
     * @param[in] _countryCode The country code to match */
    int hasCountryCode(const char _countryCode[5]);
    
    /**@brief The string to check 
     * @return 0 if the string matches, -1 if it does not, -2 on error */
    int matches(const sentence & _stringToCheck);
    
private:
    UnicodeString   m_compulsoryCharacters; // characters that HAVE to be in the sentence
    UnicodeString * m_allowedCharacters; //characters that may appear in the sentence
    char            m_hasCountryCode[5]; //the country code to match
};

inline
int selecter::setMustContainCharacters(const std::string & _characters)
{
    return setMustContainCharacters(_characters.c_str());
}
#endif //SELECTER_H
