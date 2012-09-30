#include "prec.h"
#include "parser.h"
NAMESPACE_START

parser::parser( char * _begin, char * _end )
    :m_dataset( nullptr )
    ,m_mapBegin( _begin )
    ,m_mapEnd( _end )
{
    qlog::trace << "parser::parser(const std::string&)\n";
}

// -------------------------------------------------------------------------- //


NAMESPACE_END
