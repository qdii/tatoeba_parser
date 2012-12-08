#include "prec_library.h"
#include <string.h>
#include "tatoparser/sentence.h"
#include "tatoparser/linkset.h"
#include "tatoparser/dataset.h"
#include "datainfo.h"

NAMESPACE_START

linkset::linkset()
    :m_links()
    ,m_offsets()
{
}

// -------------------------------------------------------------------------- //

void linkset::allocate( const datainfo & _datainfo )
{
    // prepare link array
    m_links.resize( _datainfo.m_nbLinks + _datainfo.m_nbLinks/100, 0 );

    // prepare ptrs array
    m_offsets.resize( _datainfo.m_highestId + 1 );

    llog::info << "Allocated "
               << ( m_links.capacity()*sizeof( sentence::id ) +
                    m_offsets.capacity()*sizeof( size_t ) ) / ( 1024*1024 )

               << " MB to store links\n";
}

// -------------------------------------------------------------------------- //

sentence::id getFirstSentenceTranslation(
    const dataset & _dataset,
    const linkset & _linkset,
    sentence::id _sentence,
    const std::string & _lang
)
{
    auto allTranslations = _linkset.getLinksOf( _sentence );

    for( auto iter = allTranslations.first; iter != allTranslations.second; ++iter )
    {
        assert( *iter != sentence::INVALID_ID);
        if ( _dataset[*iter] && _lang == _dataset[*iter]->lang() )
        {
            return *iter;
        }
    }

    return sentence::INVALID_ID;
}

//---------------------------------------------------------------------------- //

sentence::id linkset::getHighestSentenceId() const
{
    const auto highestIdIterator = std::max_element( m_links.begin(), m_links.end() );
    return highestIdIterator == m_links.end() ? /* sentence::INVALID_ID */ 0 : *highestIdIterator;
}

NAMESPACE_END
