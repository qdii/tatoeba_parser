#include "prec.h"
#include "sentence.h"
#include <string.h>
#include "linkset.h"
#include "dataset.h"

NAMESPACE_START

linkset::linkset( size_t, size_t _nbLinks, size_t _highestId )
    :m_links()
    ,m_pointers()
{
    // prepare link array
    m_links.resize( _nbLinks + _nbLinks/100, 0 );

    // prepare ptrs array
    m_pointers.resize( _highestId+1, 0 );

    qlog::info << "Allocated "
        << ( m_links.capacity()*sizeof( sentence::id ) +
             m_pointers.capacity()*sizeof( size_t ) ) / ( 1024*1024 )

        << " MB to store links\n";
}

// -------------------------------------------------------------------------- //



NAMESPACE_END
