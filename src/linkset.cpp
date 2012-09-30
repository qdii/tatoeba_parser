#include "prec.h"
#include "sentence.h"
#include <string.h>
#include "linkset.h"
#include "dataset.h"

NAMESPACE_START

linkset::linkset( size_t _nbSentences, size_t _nbLinks )
    :m_links()
    ,m_pointers()
{
    // prepare link array
    static const size_t LINK_ARRAY_SIZE = ( _nbLinks + _nbLinks/100 ) * sizeof( sentence::id );
    m_links.resize( LINK_ARRAY_SIZE, 0 );

    // prepare ptrs array
    static const size_t POINTER_ARRAY_SIZE = _nbSentences * sizeof( sentence::id ) ;
    m_pointers.resize( POINTER_ARRAY_SIZE, nullptr );

    qlog::info << "Allocated " << ( LINK_ARRAY_SIZE + POINTER_ARRAY_SIZE ) / ( 1024*1024 )<< " MB to store links\n";
}

// -------------------------------------------------------------------------- //



NAMESPACE_END
