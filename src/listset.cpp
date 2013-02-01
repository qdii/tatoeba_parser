#include "prec_library.h"
#include "tatoparser/sentence.h"
#include "tatoparser/listset.h"

NAMESPACE_START


// -------------------------------------------------------------------------- //
void listset::addSentenceToList( sentence::id _id, const std::string & _listName )
{
    using namespace llog;
    const list_hash & hash = computeHash( _listName );
    llog::debug << "adding sentence " << color(green) << _id << color() << " to list " << color(yellow) << _listName << color() << " (hash: " << color(cyan) << hash << color() << ")\n";
    offset off = findOffset( hash );

    // the offset has not been found, so we create an entry
    if( static_cast<offset>( -1 ) == off )
        addNewList( hash );

    assert( static_cast<offset>( -1 ) != findOffset( hash ) );

    off = findOffset( hash );
    list & l = getList( off );
    l.push_back( _id );
}

// -------------------------------------------------------------------------- //
bool listset::isSentenceInList( sentence::id _id, const std::string & _listName ) const
{
    return isSentenceInList( _id, computeHash( _listName ) );
}

// -------------------------------------------------------------------------- //
bool listset::isSentenceInList( sentence::id _id, list_hash _hash ) const
{
    bool found = false;
    const offset off = findOffset( _hash );

    if( static_cast<offset>( -1 ) != off )
    {
        const list & l = getList( off );
        found = std::find( l.begin(), l.end(), _id ) != l.end();
    }

    return found;
}

// -------------------------------------------------------------------------- //
void listset::addNewList( list_hash _hash )
{
    assert( m_offsets.find( _hash ) == m_offsets.end() );
    m_lists.push_back( list() );
    try
    {
        m_offsets.insert( std::make_pair( _hash, m_lists.size() - 1 ) );
    }
    catch( ... )
    {
        m_lists.pop_back();
        throw;
    }
}

// -------------------------------------------------------------------------- //
listset::list_hash listset::computeHash( const std::string & _listName )
{
    static std::hash<std::string> hasher;
    return hasher( _listName );
}

// -------------------------------------------------------------------------- //
listset::offset listset::findOffset( list_hash _hash ) const
{
    const auto ret = m_offsets.find( _hash );
    return ( m_offsets.end() == ret ) ? static_cast<offset>( -1 ) : ret->second;
}

// -------------------------------------------------------------------------- //
const listset::list & listset::getList( const offset & _offset ) const
{
    assert( _offset < m_lists.size() );
    return m_lists[ _offset ];
}

// -------------------------------------------------------------------------- //
listset::list & listset::getList( const offset & _offset )
{
    assert( _offset < m_lists.size() );
    return m_lists[ _offset ];
}

// -------------------------------------------------------------------------- //
bool listset::doesListExist( const std::string & _listName ) const
{
    const list_hash hash = computeHash( _listName );
    const offset off = findOffset( hash );
    return off != static_cast< offset > (-1);
}

NAMESPACE_END
