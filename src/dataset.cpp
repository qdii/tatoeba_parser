#include "prec_library.h"
#include "tatoparser/dataset.h"

NAMESPACE_START

// -------------------------------------------------------------------------- //

dataset::dataset()
    :m_allSentences()
    ,m_fastAccess()
{
}

// -------------------------------------------------------------------------- //

void dataset::allocate( const datainfo & _info )
{
    assert( _info.m_nbSentences > 0 );
    m_allSentences.reserve( _info.m_nbSentences );

    llog::info << "Allocated " << ( m_allSentences.capacity() * sizeof( sentence ) )/ ( 1024*1024 )<< " MB for sentences.\n";
}

// -------------------------------------------------------------------------- //

void dataset::prepare( const datainfo & _info ) TATO_RESTRICT
{
    m_fastAccess.resize( _info.m_highestId + 1, static_cast<size_t>( -1 ) );
    const size_t nbSentences = m_allSentences.size();

    for( size_t index = 0; index < nbSentences; ++index )
    {
        const sentence & TATO_RESTRICT curSentence = m_allSentences[ index ];
        assert( curSentence.getId() != sentence::INVALID_ID );
        assert( curSentence.getId() < static_cast<sentence::id>( m_fastAccess.size() ) );
        m_fastAccess[curSentence.getId()] = index;
    }
}

// -------------------------------------------------------------------------- //

NAMESPACE_END
