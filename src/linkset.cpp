#include "prec.h"
#include "sentence.h"
#include <string.h>
#include "linkset.h"
#include "dataset.h"
#include "datainfo.h"

NAMESPACE_START

linkset::linkset()
    :m_links()
    ,m_pointers()
{
}

// -------------------------------------------------------------------------- //

void linkset::allocate( const datainfo & _datainfo )
{
    // prepare link array
    m_links.resize( _datainfo.m_nbLinks + _datainfo.m_nbLinks/100, 0 );

    // prepare ptrs array
    m_pointers.resize( _datainfo.m_highestId + 1, 0 );

    qlog::info << "Allocated "
               << ( m_links.capacity()*sizeof( sentence::id ) +
                    m_pointers.capacity()*sizeof( size_t ) ) / ( 1024*1024 )

               << " MB to store links\n";
}

// -------------------------------------------------------------------------- //



NAMESPACE_END
