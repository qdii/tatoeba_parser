#include "prec_library.h"
#include "tatoparser/dataset.h"
#include "datainfo.h"

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
    allocate( _info.m_nbSentences );
}

// -------------------------------------------------------------------------- //

void dataset::allocate( const size_t _nbSentences )
{
    assert( _nbSentences != 0 );
    m_allSentences.reserve( _nbSentences );
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
//
void dataset::merge( dataset && _other )
{
    m_allSentences.insert(
        m_allSentences.end(),
        std::make_move_iterator( _other.m_allSentences.begin() ),
        std::make_move_iterator( _other.m_allSentences.end() )
    );
    m_fastAccess.insert(
        m_fastAccess.end(),
        std::make_move_iterator( _other.m_fastAccess.begin() ),
        std::make_move_iterator( _other.m_fastAccess.end() )
    );
}
NAMESPACE_END
