#include "prec.h"
#include <include/tatoparser/namespace.h>
#include <include/tatoparser/sentence.h>
#include "display.h"
#include "qlog.hpp"

NAMESPACE_START

cout_display::cout_display( const bool _colored,
                            const std::string & _separator )
    : m_colored { _colored }
    , m_separator { _separator }
{
}

void cout_display::writeSentence(
    const sentence & _sentence, const flag _flags, const unsigned _lineNumber,
    const sentence * _translation )
{
    using namespace qlog;

    // option -n
    if( _flags & DISPLAY_LINE_NUMBER )
        qlog::cout << _lineNumber << m_separator;

    // option -i
    if( _flags & DISPLAY_IDS )
    {
        if ( m_colored )
            qlog::cout << color( yellow ) << _sentence.getId() << color() << m_separator;
        else
            qlog::cout << _sentence.getId() << m_separator;
    }

    // option --display-lang
    if( _flags & DISPLAY_LANGUAGES )
    {
        if ( m_colored )
            qlog::cout << color( red ) << _sentence.lang() << color() << m_separator;
        else
            qlog::cout << _sentence.lang() << m_separator;
    }

    // display the sentence
    qlog::cout << _sentence.str();

    // display a translation if it was requested
    if( _flags & DISPLAY_FIRST_TRANSL && _translation )
    {
        assert( _translation->getId() != sentence::INVALID_ID );

        // display the translation
        if ( m_colored )
            qlog::cout << m_separator << color( cyan ) << _translation->str() << color();
        else
            qlog::cout << m_separator << _translation->str();
    }
    qlog::cout << '\n';
}
NAMESPACE_END
