#include "prec.h"
#include "file_mapper.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

NAMESPACE_START

fileMapper::fileMapper( const std::string & _filename, bool _rdOnly ) throw (invalid_file, map_failed)
    :m_fd( open( _filename.c_str(), _rdOnly ? O_RDONLY : O_RDWR ) )
    ,m_size( 0 )
    ,m_region( nullptr )
{
    if( m_fd != 0 )
    {
        // getting the file siwe
        struct stat st;
        const int ret = fstat( m_fd, &st );

        if( ret == -1 )
            throw invalid_file(_filename);

        m_size = st.st_size;

        // mapping the region
        m_region = static_cast<char *>(
                       mmap(
                           nullptr, m_size, 
                           _rdOnly ? PROT_READ : PROT_READ | PROT_WRITE,
                           MAP_PRIVATE, m_fd, 0
                       )
                   );

        if( m_region == nullptr )
            throw map_failed();
            
        qlog::info << "mapped " << _filename << " to " << static_cast<void*>(m_region) << std::endl;
    }
    else
        throw invalid_file(_filename);
}

fileMapper::~fileMapper()
{
    if( m_region )
    {
        munmap( m_region, m_size );
        qlog::info << "unmapped region " << static_cast<void*>(m_region) << std::endl;
    }

}

NAMESPACE_END
