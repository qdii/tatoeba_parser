#include "prec.h"
#include "fast_sentence_parser.h"
#include <assert.h>

NAMESPACE_START

fastSentenceParser::fastSentenceParser( char * _begin, char * _end )
    :parser( _begin, _end )
{
}

int fastSentenceParser::start() throw()
{
    register char * __restrict ptr = getMapBegin();
    register char * const ptrEnd = getMapEnd();
    dataset * __restrict sentenceDataset = getDataset();

    if( ptr == 0 || ptrEnd == ptr )
        return 0;

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
    unsigned nbSentences = 0;

    for( ; ptr != ptrEnd; ++ptr )
    {
        c = *ptr;

        switch( state )
        {

        case DATA_WAIT_FOR_EOL:
            if( __builtin_expect( c == '\n', false ) )
            {
                sentenceDataset->addSentence(id, lang, data);
                assert( id != sentence::INVALID_ID );
                assert( lang != nullptr );
                assert( data != nullptr );
                data = nullptr;
                lang = nullptr;
                id =  sentence::INVALID_ID;

                *ptr = '\0';
                state = ID;
                ++nbSentences;
            }

            break;

        case LANG_WAIT_FOR_TAB:
            if( __builtin_expect( c == '\t', false ) )
            {
                *ptr = '\0';
                state = DATA;
            }

            break;

        case ID:
            id = 0;
            state = ID_WAIT_FOR_TAB;
            //break;

        case ID_WAIT_FOR_TAB:
            if( c != '\t' )
                id = 10 * id + ( c - '0' );
            else
                state = LANG;

            break;

        case LANG:
            lang = ptr;
            state = LANG_WAIT_FOR_TAB;
            break;


        case DATA:
            data = ptr;
            state = DATA_WAIT_FOR_EOL;
            break;
        }
    }

    qlog::info << "parsed " << nbSentences << " sentences\n";
    return static_cast<int>( nbSentences );
}

NAMESPACE_END
