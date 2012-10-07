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

/**@struct map_failed
 * @brief An exception thrown when mapping the file fails */
struct map_failed
{
};

/**@struct fileMapper
 * @brief Maps a file to memory */
struct fileMapper
{
    /**@brief Constructs a fileMapper by setting up the mapping
     * @throw invalid_file If the file can’t be opened, map_failed if mapping
     * the file fails. */
    fileMapper( const std::string & _fileName, bool _rdOnly = false );

    /**@brief Unset a mapping */
    ~fileMapper() TATO_NO_THROW;

    /**@brief Returns a pointer to the first byte of the mapping */
    char * getRegion()  { return m_region; }

    /**@brief Returns a const pointer to the first byte of the mapping */
    const char * getRegion() const { return m_region; }

    /**@brief Returns the size of the mapping in bytes */
    size_t getSize() const { return m_size; }

public: // support for iterators
    typedef char *          iterator;
    typedef const char *    const_iterator;

    iterator        begin()         { return getRegion(); }
    const_iterator  begin() const   { return getRegion(); }

    iterator        end()           { return getRegion() + getSize(); }
    const_iterator  end() const     { return getRegion() + getSize(); }

private:
    size_t m_size;
    char * m_region;

private:
    fileMapper( const fileMapper & ) = delete;
    fileMapper & operator=( const fileMapper & ) = delete;
};

NAMESPACE_END

#endif // FILE_MAPPER_H
