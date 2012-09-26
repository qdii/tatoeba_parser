#ifndef FILE_MAPPER_H
#define FILE_MAPPER_H

NAMESPACE_START

/**@struct invalid_file
 * @brief An exception thrown if the file cannot be mapped */
struct invalid_file
{
    invalid_file( const std::string & _filename ): m_filename( _filename ) { }
    std::string m_filename;
};

struct map_failed
{
};

/**@struct fileMapper
 * @brief Maps a file to memory */
struct fileMapper
{
    fileMapper( const std::string & _fileName, bool _rdOnly = false )
        throw( invalid_file, map_failed );
    ~fileMapper();

    char * getRegion()  { return m_region; }
    const char * getRegion() const { return m_region; }
    size_t getSize() const { return m_size; }

private:
    int m_fd;
    size_t m_size;
    char * m_region;

private:
    fileMapper( const fileMapper & ) = delete;
    fileMapper & operator=( const fileMapper & ) = delete;
};

NAMESPACE_END

#endif // FILE_MAPPER_H
