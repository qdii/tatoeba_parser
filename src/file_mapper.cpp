#include "prec_library.h"
#include "file_mapper.h"

#if HAVE_SYS_MMAN_H == 1
#   include <sys/mman.h>
#   include <sys/stat.h>
#   include <sys/types.h>
#   include <unistd.h>
#   include <fcntl.h>
#else
#   include <fstream>
#endif

#pragma GCC visibility push(hidden)

NAMESPACE_START

#if HAVE_SYS_MMAN_H == 1
fileMapper::fileMapper( const std::string & _filename, bool _rdOnly )
    :m_size( 0 )
    ,m_region( nullptr )
{
    const int fileDescriptor
        = open( _filename.c_str(), _rdOnly ? O_RDONLY : O_RDWR );

    if( fileDescriptor != 0 )
    {
        // getting the file size
        struct stat st;
        const int ret = fstat( fileDescriptor, &st );

        if( ret == -1 )
            throw invalid_file( _filename );

        m_size = st.st_size;


        // map the region
        m_region =
            static_cast<char *>(
                mmap(
                    nullptr, m_size,
                    _rdOnly ? PROT_READ : PROT_READ | PROT_WRITE,
                    MAP_PRIVATE, fileDescriptor, 0
                )
            );

        if( m_region == reinterpret_cast<void *>( -1 ) )
            throw map_failed();

        llog::info << "mapped " << _filename << " to " << static_cast<void *>( m_region ) << std::endl;
    }
    else
        throw invalid_file( _filename );
}

// -------------------------------------------------------------------------- //

fileMapper::~fileMapper() TATO_NO_THROW
{
    // unmap the region if it was mapped
    if( m_region )
    {
        munmap( m_region, m_size );
        llog::info << "unmapped region " << static_cast<void *>( m_region ) << std::endl;
    }
}

// -------------------------------------------------------------------------- //

#else // HAVE_SYS_MMAN_H

fileMapper::fileMapper( const std::string & _filename, bool /* _rdOnly */ )
    :m_size( 0 )
    ,m_region( nullptr )
{
    // get the file size
    std::ifstream filestream(
        _filename.c_str(),
        std::ios_base::binary | std::ios_base::in | std::ios_base::ate
    );

	if (!filestream.is_open())
		throw invalid_file( _filename );

	filestream.exceptions( std::ifstream::failbit | std::ifstream::badbit );
	const std::streamoff filesize = filestream.tellg();
	char * region = nullptr;

    // copy the file to memory
    try
    {
        region = new char[filesize + 1];
    }
    catch( const std::bad_alloc & )
    {
        llog::warning << "Failed to allocate " << filesize  + 1<< " to map " << _filename << '\n';
        throw map_failed();
    }

    try
    {
        filestream.seekg(std::ios_base::beg);
        filestream.read( region, filesize );
        filestream.close();
    }
    catch( const std::ifstream::failure & )
    {
        delete [] region;
        throw invalid_file( _filename );
    }

    region[filesize] = '\0';

    // everything seems fine, lets build the object
    m_region = region;

    m_size = filesize;
}

// -------------------------------------------------------------------------- //

fileMapper::~fileMapper() TATO_NO_THROW
{
    delete [] m_region;
}
#endif // HAVE_SYS_MMAN_H

NAMESPACE_END

#pragma GCC visibility pop
