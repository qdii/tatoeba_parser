#ifndef TAGSET_H
#define TAGSET_H

#include <cstdint> // for tagId
#include <functional> // for struct hash<>
#include <map>
#include "sentence.h"

NAMESPACE_START

/**@struct tagset
 * @brief Stores the tags associated to the sentences. */
struct tagset
{
    tagset();
    ~tagset() { }

    // An unique identifier representing the tag of the sentence
    typedef uint16_t tagId;
    static const tagId INVALID_TAGID = 0;

    /**@brief Returns an id corresponding to a tag
     * @param[in] _tagName The name of the tag, for instance "maths"
     * @return INVALID_ID if _tagName is a null string, a valid tagId if not */
    tagId getTagId( const std::string & _tagName );

    void tagSentence( sentence::id _id, const std::string & _tagName );

    bool isSentenceTagged( sentence::id _id, tagId _tag );

private:
    void tagSentence( sentence::id _id, tagId _tag);

    typedef std::vector<sentence::id> sentenceList;
    typedef std::map<tagId, sentenceList> tagToSentencesMap;

    tagToSentencesMap m_tagToSentences;
    std::map<std::string, tagId> m_nameToId;
};

// -------------------------------------------------------------------------- //

inline
tagset::tagId tagset::getTagId( const std::string & _tagName )
{
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
    return tagSentence( _id, getTagId( _tagName ) );
}
NAMESPACE_END

#endif //TAGSET_H
