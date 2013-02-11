#ifndef LIBTATOPARSER_FAST_SENTENCE_ADV_PARSER_H
#define LIBTATOPARSER_FAST_SENTENCE_ADV_PARSER_H

#include <boost/spirit/include/qi.hpp>
#include "tatoparser/namespace.h"

#pragma GCC visibility push(hidden)

NAMESPACE_START

/**@brief Parses the sentences_detailed .csv out of a buffer
 * @tparam iterator An input/output iterator to read and write into the buffer
 * @struct fastDetailedParser */
template<typename iterator>
struct fastDetailedParser
{
    /**@brief Constructs a fastSentenceParser
     * @param _begin An iterator to the first character of the buffer
     * @param _end An iterator to the end of the buffer */
    fastDetailedParser( iterator _begin, iterator _end );

    /**@brief Start parsing the buffer and fill up a dataset with the sentences
      *@return The number of sentences parsed. */
    size_t          start( dataset & _data ) TATO_NO_THROW;

    /**@brief Returns the number of lines in the buffer
      *@warning If some lines are fucked up in the file, the number of sentences
      *         will be inferior to the number of lines. */
    size_t          countLines() const;

    /**@brief Estimate the number of lines in the buffer*/
    size_t          countLinesFast() const;

private:
    iterator m_begin, m_end;
};

// -------------------------------------------------------------------------- //
template<typename iterator>
fastDetailedParser<iterator>::fastDetailedParser( iterator _begin, iterator _end )
    :m_begin( _begin )
    ,m_end( _end )
{
}

// -------------------------------------------------------------------------- //
template<typename iterator>
size_t fastDetailedParser<iterator>::start( dataset & _data ) TATO_NO_THROW
{
    namespace qi = boost::spirit::qi;

    size_t nbSentences = 0;
    size_t line = 1;

    // memory to parse
    auto begin = m_begin;
    auto const end = m_end;

    // variables to store parsed data
    sentence::id id;
    boost::iterator_range<iterator> langRange;
    boost::iterator_range<iterator> sentenceRange;
    boost::iterator_range<iterator> authorRange;
    boost::iterator_range<iterator> creationDateRange;
    boost::iterator_range<iterator> lastModifiedDateRange;

    dataset temporarySentenceContainer;
    bool parsingFailed = false;

    while( true )
    {
        if( qi::parse( begin, end, (
            // grammar for a single line of CSV
            // sentence ID
            qi::uint_ >> '\t' >>

            // language: 3- or 4-character code, "\N" or nothing
            qi::raw[qi::repeat( 3,4 )[qi::ascii::lower] | "\\N" | qi::eps] >> '\t' >>

            // sentence: a non-empty string of characters till the next tab
            qi::raw[+~qi::char_( '\t' )]  >> '\t' >>

            // author
            qi::raw[+~qi::char_( '\t' )]  >> '\t' >>

            // creation date
            qi::raw[+~qi::char_( '\t' )]  >> '\t' >>

            // last modified date
            qi::raw[+~qi::char_( '\n' )]  >> '\n'
        ), id, langRange, sentenceRange, authorRange,
        creationDateRange, lastModifiedDateRange ) )
        {
            // ok, we managed to parse a sentence.
            // change separators into string endings
            *( langRange.end() ) = '\0';
            *( sentenceRange.end() ) = '\0';
            *( authorRange.end() ) = '\0';
            *( creationDateRange.end() ) = '\0';
            *( lastModifiedDateRange.end() ) = '\0';

            try
            {
                temporarySentenceContainer.addSentence(
                    id, langRange.begin(), sentenceRange.begin(),
                    authorRange.begin(), creationDateRange.begin(),
                    lastModifiedDateRange.begin()
                );
            }
            catch( const std::bad_alloc & )
            {
                llog::error << "Not enough memory.\n";
                parsingFailed = true;
                break;
            }

            nbSentences++;
        }
        else if( begin != end )
        {
            // we failed at parsing the sentence, and we're not at the end of
            // the file yet
            llog::warning << "Failed to parse sentence from line " << line << std::endl;

            // skip over the nearest \n and try again.
            while( *( begin++ ) != '\n' );
        }
        else
        {
            // we're at the end of file. finish the job
            break;
        }

        line++;
    }

    if( !parsingFailed )
        _data = std::move( temporarySentenceContainer );

    return nbSentences;
}

// -------------------------------------------------------------------------- //

template<typename iterator>
size_t fastDetailedParser<iterator>::countLines() const
{
    const size_t nbSentences =
        std::count( m_begin, m_end, '\n' );
    llog::info << "number of sentences: " << nbSentences << '\n';
    return nbSentences;
}

// -------------------------------------------------------------------------- //

template<typename iterator>
size_t fastDetailedParser<iterator>::countLinesFast() const
{
    const size_t nbChars =
        reinterpret_cast<size_t>( &*m_end ) - reinterpret_cast<size_t>( &*m_begin );
    static const size_t AVERAGE_NB_BYTES_PER_SENTENCE = 100;
    llog::info << "estimated number of sentences: " << nbChars / AVERAGE_NB_BYTES_PER_SENTENCE << '\n';
    return nbChars / AVERAGE_NB_BYTES_PER_SENTENCE;
}

NAMESPACE_END

#pragma GCC visibility pop

#endif // LIBTATOPARSER_FAST_SENTENCE_ADV_PARSER_H
