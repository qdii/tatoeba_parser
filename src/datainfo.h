#ifndef LIBTATOPARSER_DATAINFO_H
#define LIBTATOPARSER_DATAINFO_H

#include "tatoparser/namespace.h"
#include "tatoparser/sentence.h"
#include "fast_link_parser.h"

NAMESPACE_START

/**@brief Retains some informations about the parsed files */
struct datainfo
{
    // how many sentences where contained in sentences.csv
    size_t m_nbSentences;


    // number of lines in links.csv
    typename fastLinkParser<char *>::nb_of_lines m_nbLinks;

    // the id which had the greatest value out of all the parsed sentences
    sentence::id m_highestId;
};

NAMESPACE_END

#endif //LIBTATOPARSER_DATAINFO_H
