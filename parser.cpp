#include "prec.h"
#include "parser.h"
NAMESPACE_START

parser::parser( const fileMapper & _map )
    :m_fileMap( _map )
    ,m_dataset( nullptr )
{
    qlog::trace << "parser::parser(const std::string&)\n";
}

// -------------------------------------------------------------------------- //


NAMESPACE_END
