#include "prec.h"
#include "tagset.h"

NAMESPACE_START

tagset::tagset()
    :m_tagToSentences()
    ,m_nameToId()
{
}

// -------------------------------------------------------------------------- //

bool tagset::isSentenceTagged( sentence::id _id, tagId _tag )
{
    sentenceList & TATO_RESTRICT
        allSentencesMatchingTheTag = m_tagToSentences[ _tag ];

    return std::find(
        allSentencesMatchingTheTag.begin(),
        allSentencesMatchingTheTag.end(),
        _id) != allSentencesMatchingTheTag.end();
}

// -------------------------------------------------------------------------- //

void tagset::tagSentence( sentence::id _id, tagset::tagId _newTag )
{
    m_tagToSentences[ _newTag ].push_back(_id);
}

NAMESPACE_END
