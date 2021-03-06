#ifndef TAGSET_H
#define TAGSET_H

#include <cstdint> // for tagId
#include <functional> // for struct hash<>
#include <map>
#include <vector>
#include "namespace.h"
#include "sentence.h"

NAMESPACE_START

/**@struct tagset
 * @brief Stores the tags associated to the sentences. */
struct tagset
{
    tagset();
    tagset & operator=( tagset && ) = default;

    // An unique identifier representing the tag of the sentence
    typedef uint16_t tagId;
    static const tagId INVALID_TAGID = 0;

    /**@brief Returns an id corresponding to a tag
     * @param[in] _tagName The name of the tag, for instance "maths. The name should be lower-case."
     * @return INVALID_ID if _tagName is a null string, a valid tagId if not */
    tagId getTagId( const std::string & _tagName );

    /**@brief Adds a new tag for a given sentence
     * @param[in] _id The id of the sentence
     * @param[in] _tagName the name of the tag. This should be lowercase.
     * @throw std::bad_alloc */
    void tagSentence( sentence::id _id, const std::string & _tagName );

    bool isSentenceTagged( sentence::id _id, tagId _tag );

private:
    void tagSentence( sentence::id _id, tagId _tag );

    typedef std::vector<sentence::id> sentenceList;
    typedef std::map<tagId, sentenceList> tagToSentencesMap;

    tagToSentencesMap m_tagToSentences;
    std::map<std::string, tagId> m_nameToId;
};

// -------------------------------------------------------------------------- //

std::string toLower( const std::string & _name );

// -------------------------------------------------------------------------- //

inline
tagset::tagId tagset::getTagId( const std::string & _tagName )
{
    assert( toLower( _tagName ) == _tagName );

    static tagId idGenerator = 0;

    tagId & ret = m_nameToId[_tagName];

    if( ret == 0 )
        ret = ++idGenerator;

    return ret;
}

// -------------------------------------------------------------------------- //

inline
void tagset::tagSentence( sentence::id _id, const std::string & _tagName )
{
    return tagSentence( _id, getTagId( toLower( _tagName ) ) );
}
NAMESPACE_END

#endif //TAGSET_H
