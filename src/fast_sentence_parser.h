#ifndef FAST_SENTENCE_PARSER_H
#define FAST_SENTENCE_PARSER_H

#include "dataset.h"
#define TATO_DIAGNOZE
NAMESPACE_START

/**@brief Parses the sentences out of a buffer
 * @tparam iterator An input/output iterator to read and write into the buffer
 * @struct fastSentenceParser */
template<typename iterator>
struct fastSentenceParser
{
    /**@brief Constructs a fastSentenceParser
     * @param _begin An iterator to the first character of the buffer
     * @param _end An iterator to the end of the buffer */
    fastSentenceParser( iterator _begin, iterator _end );

    /**@brief Start parsing the buffer and fill up a dataset with the sentences
      *@return The number of sentences parsed. */
    size_t          start( dataset & _data ) TATO_NO_THROW;

    /**@brief Returns the number of lines in the buffer
      *@warning If some lines are fucked up in the file, the number of sentences
      *         will be inferior to the number of lines. */
    size_t          countLines();

private:
    iterator m_begin, m_end;
};

// -------------------------------------------------------------------------- //

template<typename iterator>
fastSentenceParser<iterator>::fastSentenceParser( iterator _begin, iterator _end )
    :m_begin( _begin )
    ,m_end( _end )
{
}

// -------------------------------------------------------------------------- //

template<typename iterator>
size_t fastSentenceParser<iterator>::countLines()
{
    const size_t nbSentences =
        std::count( m_begin, m_end, '\n' );
    qlog::info << "number of sentences: " << nbSentences << '\n';
    return nbSentences;
}

// -------------------------------------------------------------------------- //

template<typename iterator>
size_t fastSentenceParser<iterator>::start( dataset & __restrict _data ) TATO_NO_THROW
{
    size_t nbSentences = 0;

    register iterator __restrict    ptr         = m_begin;
    register iterator const         ptrEnd      = m_end;

    if( ptr == 0 || ptrEnd == ptr )
        return 0;

    // the parser is a state machine that goes through those states:
    static const int ID=0;
    static const int LANG=1;
    static const int DATA=2;
    static const int ID_WAIT_FOR_TAB=4;
    static const int LANG_WAIT_FOR_TAB=5;
    static const int DATA_WAIT_FOR_EOL=7;

    // parse it
    char * lang = nullptr;
    char * data = nullptr;
    char c = '\0';
    int state = ID;
    sentence::id id = sentence::INVALID_ID;

#ifdef TATO_DIAGNOZE
    size_t line = 1;
#endif

    for( ; ptr != ptrEnd; ++ptr )
    {
        c = *ptr;

#ifdef TATO_DIAGNOZE
        if (c == '\n')
            ++line;
#endif

        switch( state )
        {

        case DATA_WAIT_FOR_EOL:
            assert( c != '\t' );

            if( __builtin_expect( c == '\n', false ) )
            {

                _data.addSentence( id, lang, data );
                ++nbSentences;

                assert( id != sentence::INVALID_ID );
                assert( lang != nullptr );
                assert( data != nullptr );
                data = nullptr;
                lang = nullptr;
                id =  sentence::INVALID_ID;

                *ptr = '\0';
                state = ID;
            }

            break;

        case LANG_WAIT_FOR_TAB:
            if( __builtin_expect( c == '\t', false ) )
            {
                *ptr = '\0';
                state = DATA;
            }

#ifdef TATO_DIAGNOZE
            if( c == '\n' )
            {
                qlog::warning << "line " << line << " is ill-formed\n";
                state = ID;
            }
#endif

            break;

        case ID:
            id = 0;
            state = ID_WAIT_FOR_TAB;
            //break;

        case ID_WAIT_FOR_TAB:
            assert( c != '\n' );

            if( c != '\t' )
                id = 10 * id + ( c - '0' );
            else
                state = LANG;

            break;

        case LANG:
            assert( c != '\n' );

#ifdef TATO_DIAGNOZE
            if( c == '\t' )
            {
                qlog::warning << "line " << line << " is ill-formed\n";
                *ptr = '\0';
            }
#endif

            lang = ptr;
            state = LANG_WAIT_FOR_TAB;
            break;


        case DATA:
            assert( c != '\t' );
            data = ptr;
            state = DATA_WAIT_FOR_EOL;
            break;
        }
    }

    return nbSentences;
}


NAMESPACE_END
#endif // FAST_SENTENCE_PARSER_H
