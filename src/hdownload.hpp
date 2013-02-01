/**@file hdownload.hpp
 * @author Victor Lavaud <victor.lavaud@gmail.com>
 * @version 1.0 */

/**@mainpage
 * DESCRIPTION
 * -----------
 * hdownload is a header-only library that provides facilities to download files
 * from the internet. It can be seen as a convenient C++ wrapper for curl.
 *
 * TUTORIAL
 * --------
 * <h3>1. A simple download</h3>
 * Let’s start by downloading the contents of a webpage. It is trivial.
 * @code{.cpp}
 * #include "hdownload.hpp"
 *
 * int main()
 * {
 *   // initializes the downloader and points it to google search page
 *   hdl::downloader dl( "www.google.com" );
 *
 *   // fetches the page and prints it out on the standard output
 *   dl.start( std::cout );
 * }
 * @endcode
 * The \c start() function will not return until the contents have been pushed
 * into the output stream (\c std::cout, here). The output stream is not flushed
 * for performance reasons.
 *
 * <h3>2. Being robust</h3>
 * Downloading a page can fail for many reasons, some of them are related to the
 * network, such as a website being out of order, others can be more obscure. Let’s
 * try to add some padding to our code so that those errors are handled
 * gracefully.
 * Libcurl for instance can fail to initialize. In that case an exception is
 * thrown of type \c hdl::curl_exception during the instantiation of the downloader.
 */

#ifndef HDL_HPP
#define HDL_HPP

#ifdef HDL_USE_EXCEPTIONS
#   include <HDL_ASSERT.h>
#   define HDL_ASSERT(X) assert(X)
#   define HDL_ASSERTEQ(X,Y) assert((Y) == (X))
#else
#   define HDL_ASSERT(X)
#   define HDL_ASSERTEQ(X,Y) (X)
#endif

#include <exception>
#include <curl/curl.h>
#include <string>
#include <sstream>
#include <functional>
#include <iterator>
#include <array>
#include <vector>

namespace hdl
{
// -------------------------------------------------------------------------- //
struct curl_exception : public std::exception
{
    template<typename T>
    explicit curl_exception( T && _reason )
        :m_reason( _reason )
    {
    }

    virtual const char * what() const noexcept override
    {
        return m_reason.c_str();
    }

private:
    std::string m_reason;
};

// -------------------------------------------------------------------------- //
struct download_failed : public std::exception
{
};
// -------------------------------------------------------------------------- //

static constexpr bool FOLLOW_REDIRECTION_DEFAULT = true;
static constexpr bool THROW_EXCEPTION_WHEN_DOWNLOAD_FAILS_DEFAULT = false;

// -------------------------------------------------------------------------- //
/**@struct curl_wrapper
 * @private
 * @brief Internal wrapper for curl, provides asserts, enable tracing
 * @throw curl_exception if curl init fails */
struct curl_wrapper
{
    /**@brief Constructs a curl_wrapper
     * @brief _debug Whether to add additional testings */
    curl_wrapper( bool _debug, bool _throwOnFailure )
        :m_handle( curl_easy_init() )
        ,m_debug( _debug )
        ,m_throwOnFailure( _throwOnFailure )
        ,m_buffer()
    {
        // initializing curl can fail
        if( !m_handle )
            throw curl_exception( "curl_easy_init() failed." );

        // give libcurl buffer offerings
        set_opt( CURLOPT_ERRORBUFFER, m_buffer.data() );

        if( _debug )
            enable_debug();
    }

    /**@brief Moves a curl_wrapper
     * @param[in] _obj The moved object */
    curl_wrapper( curl_wrapper && _obj )
        :m_handle( _obj.m_handle )
    {
        _obj.m_handle = nullptr;
    }

    curl_wrapper( curl_wrapper & ) = delete;
    curl_wrapper & operator=( curl_wrapper & ) = delete;

    /**@brief Assigns a moved curl_wrapper to this object
     * @param[in] _obj A destructible curl_wrapper object to assign from */
    curl_wrapper & operator=( curl_wrapper && _obj )
    {
        m_handle = _obj.m_handle;
        _obj.m_handle = nullptr;
        return *this;
    }

    /**@brief Tells curl to log at a certain location */
    void enable_debug()
    {
        set_opt( CURLOPT_VERBOSE, 1L );
        set_opt( CURLOPT_DEBUGFUNCTION, debugFunction );
        set_opt( CURLOPT_DEBUGDATA, this );
    }

    /**@brief Destructs a curl_wrapper, frees libcurl memory */
    virtual ~curl_wrapper()
    {
        if( m_handle )
            curl_easy_cleanup( m_handle );
    }

    virtual bool updateProgress( double, double, double, double )
    {
        return false;
    }

    virtual void outputDebug( const std::string & _debugString )
    {
        std::cout << _debugString;
    }

    virtual void throwExceptionOnFailure( bool _val )
    {
        m_throwOnFailure = _val;
    }

public:
    /**@brief Start fetching the page
     * @return true on success, false otherwise
     * @throw download_failed if the download fails */
    bool perform()
    {
        HDL_ASSERT( m_handle != nullptr );
        const CURLcode ret = curl_easy_perform( m_handle );

        if( ret != CURLE_OK && m_throwOnFailure )
            throw download_failed();

        return ret == CURLE_OK;
    }

    /**@brief A wrapper for curl_easy_setopt
     * @param[in] _option The option to pass
     * @param[in] _args The arguments to pass
     * @return true on success */
    template<typename... Args>
    bool set_opt( CURLoption _option, Args... _args )
    {
        HDL_ASSERT( m_handle != nullptr );
        const CURLcode ret = curl_easy_setopt( m_handle, _option, _args... );
        return ret == CURLE_OK;
    }

private:
    CURL * m_handle;
    bool m_debug;
    bool m_throwOnFailure;
    std::array< char, CURL_ERROR_SIZE > m_buffer; // for debugging purposes

public: // curl callbacks
    static int progressFunction( void * clientp, double dltotal, double dlnow, double ultotal, double ulnow );
    static size_t writeFunction( char * _ptr, size_t _size, size_t _nmemb, void * _userdata );
    static void debugFunction( CURL * _handle, curl_infotype _type,
                               char * _ptr, size_t _size, void * _userdata );
};

// -------------------------------------------------------------------------- //
/**@struct downloader
 * @brief A download session */
struct basic_downloader : protected curl_wrapper
{
    /**@brief Constructs a downloader
     * @param[in] _uri The address of the page to download the contents of.
     * @throw std::bad_alloc if no memory is avaliable,
              curl_exception if curl cannot be optimized */
    template<typename STD_STRING> explicit
    basic_downloader( STD_STRING && _uri );

    /**@brief Copying a downloader is forbidden */
    basic_downloader( const basic_downloader & ) = delete;

    /**@brief Construct-moves a downloader
     * @param[in] _obj The downloader object to move from */
    basic_downloader( basic_downloader && _obj );

    /**@brief Copying a downloader is forbidden */
    basic_downloader & operator=( const basic_downloader & ) = delete;

    /**@brief Moves a downloader
     * @return *this  */
    basic_downloader & operator=( basic_downloader && _obj );

    /**@brief Starts downloading
     * @return 0 on success, 1 on failure
     *
     * The function starts downloading the contents, and will not return
     * until the download is finished.
     *
     * @param[out] contents_ The page contents.
     *             Will be altered throughout the download.
     * @param[out] headers_ The headers of the page. Will be altered only if the
     *             download succeeds. Pass nullptr if you do not wish to receive
     *             the headers, or pass a standard container of std::string objects,
     *             and each header will append a new std::string object to it.
     * @throw      hdl::download_failed if exceptions are enabled
     * @return     true if the download succeeds, false otherwise */
    template<typename Collection>
    bool stream( std::ostream & contents_, Collection * headers_ );

    /**@brief Starts downloading
     * @return 0 on success, 1 on failure
     *
     * The function starts downloading the contents, and will not return
     * until the download is finished.
     *
     * @param[out] contents_ A valid string in which the page contents will be stored.
     *             The string will only be altered if the download succeeds.
     * @param[out] headers_ The headers of the page. Will be altered only if the
     *             download succeeds. Pass nullptr if you do not wish to receive
     *             the headers, or pass a standard container of std::string objects,
     *             and each header will append a new std::string object to it.
     * @return     true if the download succeeds, false otherwise */
    template<typename Collection>
    bool start( std::string & contents_, Collection * headers_ );

    bool start( std::string & contents );
    bool stream( std::ostream & contents );

    /**@brief Sets the behaviour when a redirect code is returned
     * @param[in] _val Set to true to follow redirections. */
    void followRedirect( bool _val ) { m_followRedirect = _val; }

    /**@brief Throws an exception when the download fails
     * @param[in] _val Set to true to receive an exception when the download fails. */
    void throwExceptionOnDownloadFailure( bool _val ) { throwExceptionOnFailure(_val); }

    /**@brief Register progress listener
     * @param[in] _listener
     * @throw std::bad_alloc if memory cannot be allocated
     *
     * The function will get updated every second with the download progress */
    typedef std::function< bool ( double dltotal, double dlnow, double ultotal, double ulnow ) > progress_fn;
    void registerProgressListener( progress_fn _listener ) { m_progressListenerList.push_back( _listener ); }

    virtual bool updateProgress( double dltotal, double dlnow, double ultotal, double ulnow )
    {
        bool userWantsToStopDownload = false;
        for( auto listener : m_progressListenerList )
        {
            userWantsToStopDownload |= listener( dltotal, dlnow, ultotal, ulnow );
        }
        return userWantsToStopDownload;
    }

private:
    std::string m_uri;
    bool m_followRedirect;
    std::vector< progress_fn > m_progressListenerList;
};

// -------------------------------------------------------------------------- //
int curl_wrapper::progressFunction(
    void * clientp, double dltotal, double dlnow,
    double ultotal, double ulnow )
{
    HDL_ASSERT( clientp );
    curl_wrapper * const wrapper =
        static_cast< curl_wrapper * >( clientp );

    return wrapper->updateProgress( dltotal, dlnow, ultotal, ulnow ) ? 1 : 0;
}

// -------------------------------------------------------------------------- //

size_t curl_wrapper::writeFunction( char * _ptr, size_t _size, size_t _nmemb, void * _userdata )
{
    HDL_ASSERT( _userdata != nullptr );
    std::ostringstream & contentsOutput = * static_cast<std::ostringstream * >( _userdata );
    contentsOutput << std::string( _ptr, _ptr + ( _size * _nmemb ) );
    return _size * _nmemb;
}

// -------------------------------------------------------------------------- //
template<typename Collection>
static size_t headerFunction( void * _ptr, size_t _size, size_t _nmemb, void * _userdata )
{
    HDL_ASSERT( _userdata != nullptr );
    Collection & allHeaders = * static_cast< Collection * >( _userdata );
    std::string header;
    size_t treated_size = _size * _nmemb;;

    try
    {
        header.reserve( _size * _nmemb );
        header.assign( static_cast<char *>( _ptr ), _size * _nmemb );
        allHeaders.push_back( std::move( header ) );
    }
    catch( const std::bad_alloc & )
    {
        treated_size  = 0;
    }

    return treated_size ;
}

// -------------------------------------------------------------------------- //

void curl_wrapper::debugFunction(
    CURL * _handle, curl_infotype _type,
    char * _ptr, size_t _size, void * _userdata )
{
    HDL_ASSERT( _userdata != nullptr );
    const std::string debugString( _ptr, _ptr + _size );
    curl_wrapper * wrap = static_cast<curl_wrapper *>( _userdata );
    wrap->outputDebug( debugString );
}

// -------------------------------------------------------------------------- //

template<typename STD_STRING>
basic_downloader::basic_downloader( STD_STRING && _uri )
    :curl_wrapper( false, THROW_EXCEPTION_WHEN_DOWNLOAD_FAILS_DEFAULT )
    ,m_uri( std::forward<STD_STRING>(_uri) )
    ,m_followRedirect( FOLLOW_REDIRECTION_DEFAULT )
{
    HDL_ASSERT( m_uri.size() );
}

// -------------------------------------------------------------------------- //
basic_downloader & basic_downloader::operator=( basic_downloader && _obj )
{
    curl_wrapper::operator=( std::move( _obj ) );
    m_uri = std::move( _obj.m_uri );

    return *this;
}

// -------------------------------------------------------------------------- //
basic_downloader::basic_downloader( basic_downloader && _obj )
    :curl_wrapper( std::move( _obj ) )
    ,m_uri( std::move( _obj.m_uri ) )
{
}
// -------------------------------------------------------------------------- //

template<typename Collection> inline
bool basic_downloader::start( std::string & contents_, Collection * headers_ )
{
    std::ostringstream contentStream;
    const bool ret = stream( contentStream, headers_ );

    if( true == ret )
        contents_ = std::move( contentStream.str() );

    return ret;
}

// -------------------------------------------------------------------------- //

template<typename Collection>
bool basic_downloader::stream( std::ostream & contents_, Collection * headers_ )
{
    static_assert( std::is_same<std::string, typename Collection::value_type >::value,
                   "Collection must be a standard container of std::string" );

    /* set URL to get */
    this->set_opt( CURLOPT_URL, m_uri.c_str() );

    // we log progress
    this->set_opt( CURLOPT_NOPROGRESS, 0L );

    this->set_opt(
        CURLOPT_PROGRESSDATA,
        this
    );

    this->set_opt(
        CURLOPT_PROGRESSFUNCTION,
        curl_wrapper::progressFunction
    );

    // send the data to the user ostringstream
    this->set_opt( CURLOPT_WRITEDATA, &contents_ );
    this->set_opt(
        CURLOPT_WRITEFUNCTION,
        curl_wrapper::writeFunction
    );

    // send the headers to the user’s collection
    if( headers_ != nullptr )
    {
        this->set_opt( CURLOPT_HEADERDATA, headers_ );
        this->set_opt( CURLOPT_HEADERFUNCTION, headerFunction<Collection> );
    }

    this->set_opt( CURLOPT_FOLLOWLOCATION, m_followRedirect ? 1 : 0 );

    // establish connection and fetch the data
    return this->perform();
}

// -------------------------------------------------------------------------- //

bool basic_downloader::start( std::string & contents )
{
    struct dummy_string_vector
    {
        typedef std::string value_type;
        void push_back( const std::string & ) { }
    };
    return start<dummy_string_vector>( contents, nullptr );
}

// -------------------------------------------------------------------------- //

bool basic_downloader::stream( std::ostream & contents )
{
    struct dummy_string_vector
    {
        typedef std::string value_type;
        void push_back( const std::string & ) { }
    };
    return stream<dummy_string_vector>( contents, nullptr );
}

// -------------------------------------------------------------------------- //

typedef basic_downloader downloader;
}
#endif // HDOWNLOAD_HPP

