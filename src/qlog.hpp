#ifndef QLOG_HPP
#define QLOG_HPP

/**
 * @author Victor Lavaud <victor.lavaud@gmail.com>
 * @mainpage
 * DESCRIPTION
 * -----------
 * qlog is a header-only library that provides facilities to log messages
 * on screen or to a file. The messages can be written on screen or on
 * file. It is written is C++03 and is working with both Windows and Linux,
 * even though some decorative features might be silently turned off when
 * unavailable on either platform.
 *
 * - C++03 fully compatible.
 * - Color output on terminals (via ANSI codes) and Windows console.
 * - Very low memory footprint.
 * - Supports underscore and blinking text on ANSI terminals.
 * - Exception-safe (no exceptions are thrown).
 * - Different levels of logging, configurable at run-time.
 * - Customizable via macro definitions.
 *
 * SIMPLE USAGE
 * ------------
 * 5 objects are available to the user to log messages, they are:
 * - qlog::debug
 * - qlog::trace
 * - qlog::info
 * - qlog::warning
 * - qlog::error
 *
 * They work the same way std::cout does, for instance:
 * @code{.cpp}
 * qlog::warning << "foo() was passed a null pointer" << std::endl;
 * @endcode
 *
 * REDIRECTING TO A FILE
 * ---------------------
 * The objects will all log to std::cout by default, but you can change this
 * behaviour by calling set_output(). Actually, there are 2 set_output functions.
 * One of them is a member function that you can call on any log object, like
 * qlog::debug.set_output( new_output ), and the other one is a global function
 * that will call set_output on every available logger.
 * Note that set_output() takes a std::ostream reference as a parameter.
 *
 * The following snippet will redirect all output to myprogram.log
 * @code{.cpp}
 * #include <fstream>
 * #include "qlog.hpp"
 * using namespace qlog;
 *
 * int foo()
 * {
 *     return 1;
 * }
 *
 * int main()
 * {
 *     std::ofstream logstream( "myprogram.log" );
 *     set_output( logstream );
 *     int ret = foo();
 *     qlog::error << "foo() returned: " << ret << std::endl;
 * }
 *
 * @endcode
 *
 * Now if you want to redirect <i>the debug level only</i> to a different file, it is also
 * possible:
 * @code{.cpp}
 * #include <fstream>
 * #include "qlog.hpp"
 * using namespace qlog;
 * using std::ofstream;
 *
 * int foo()
 * {
 *     return 0;
 * }
 *
 * int main()
 * {
 *     set_loglevel( loglevel::debug );
 *
 *     // redirecting all the messages to myprogram log
 *	   ofstream logstream( "myprogram.log" );
 *     set_output( logstream );
 *
 *     // redirecting the debug messages to debug.log
 *     ofstream debug_stream( "debug.log" );
 *     qlog::debug.set_output( debug_stream );
 *
 *     // logging some messages
 *     qlog::debug << "calling foo()" << std::endl; // this will go in debug.log
 *     int ret = foo();
 *     qlog::info << "foo() returned: " << ret << std::endl; // this goes to myprogram.log
 * }
 *
 * @endcode
 *
 * FILTERING IMPORTANT MESSAGES IN
 * -------------------------------
 *
 * You can restrict the messages output to only the important ones by calling
 * the global function set_loglevel() with one of the following parameters:
 * - loglevel::debug
 * - loglevel::trace
 * - loglevel::info
 * - loglevel::warning
 * - loglevel::error
 * - loglevel::disabled
 *
 * If you call <c>set_loglevel( loglevel::warning )</c>, only the error and warning
 * messages will be processed, the more detailed messages will be ignored.
 *
 * The special log level <c>loglevel::disabled</c> will disable all output. No
 * messages will be written after this has been set.
 *
 * PREPENDING YOUR MESSAGES WITH SOME CUSTOM TEXT
 * ----------------------
 * Something I like when I debug is to have every message clearly stating if
 * it is a warning, an error, or whatever. Normally, I'd prepend the warning
 * messages with some custom text, like <b>[ww]</b>, the error messages with
 * some other (say, <b>[EE]</b>), and the information messages with something
 * less visible, such as <b>[..]</b>, so that my log clearly displays what’s
 * important.
 * @image html client.png
 * Here is an easy way to do that:
 * @code{.cpp}
 * qlog::info.prepend() << "[..] ";
 * qlog::warning.prepend() << "[ww] ";
 * qlog::error.prepend() << "[EE] ";
 * @endcode
 *
 *
 * NAMING AND NAMESPACES
 * ---------------------
 * I normally like to ditch my objects on the global namespace but some people
 * don't. I have created a couple preprocessor directives if you want
 * to personalize the name of the objects and retrict them into a namespace. By
 * default, 5 objects are created (qlog::debug, qlog::trace, qlog::info, qlog::warning
 * and qlog::error), and they are thrown into the global namespace. Now if you
 * want them to be called debug, trace, info, warning and error and to belong
 * to the namespace log, what you can do is:
 * @code{.cpp}
 * #define QLOG_NAMESPACE logging
 *
 * #define QLOG_NAME_LOGGER_DEBUG deb
 * #define QLOG_NAME_LOGGER_TRACE tra
 * #define QLOG_NAME_LOGGER_INFO inf
 * #define QLOG_NAME_LOGGER_WARNING warn
 * #define QLOG_NAME_LOGGER_ERROR err
 * #include "qlog.hpp"
 *
 * int main()
 * {
 *   logging::warn << "here we go!" << std::endl;
 *   return 0;
 * }
 * @endcode
 *
 * TIPS
 * ----
 * A handy feature is the possibility to disable the logging easily:
 * Instead of writing:
 * @code{.cpp}
 * if ( foo() != SUCCESS )
 * {
 *    qlog::warning << "problem!" << std::endl;
 * }
 * @endcode
 * You can write:
 * @code{.cpp}
 * qlog::warning( foo() != SUCCESS ) << "problem" << std::endl;
 * @endcode
 *
 * WRITING FANCY MESSAGES
 * ----------------------
 *
 * <h3>Adding some colors</h3>
 *
 * On both Windows and Linux, messages can be output in different colors.
 * The library provides a class called \c color which allows the user to change
 * the font color of the message he wants to output, as well as the background
 * color.
 *
 * @code{.cpp}
 * #include "qlog.hpp"
 * using namespace qlog;
 *
 * int main()
 * {
 *     qlog::error << color(red) << "Something went wrong" << color() << '\n';
 * }
 * @endcode
 *
 * Many colors are available, but not all of them are supported by Windows.
 * qlog namespace includes:
 * - black
 * - red
 * - green
 * - yellow
 * - blue
 * - magenta
 * - cyan
 * - white
 * - gray.
 *
 * They can all be used both a font color, or to fill the background.
 * For instance, to display a yellow message over a blue background, type:
 *
 * @code{.cpp}
 * #include "qlog.hpp"
 * using namespace qlog;
 *
 * int main()
 * {
 *     qlog::error << color(yellow, blue) << "Yellow message over blue" << color() << '\n';
 * }
 * @endcode
 *
 * You might have noticed that I used the special symbol @c qlog::color() which
 * role is to restore the terminal settings to default.
 *
 * <h3>Stressing important messages</h3>
 *
 * Both Windows and Linux can use a <b>bold font</b> to draw attention to particular
 * messages. To activate this function, add "true" as a color parameter just like this:
 *
 *
 * @code{.cpp}
 * #include "qlog.hpp"
 * using namespace qlog;
 *
 * int main()
 * {
 *     qlog::error << color(red, true) << "Something went wrong" << color() << '\n';
 * }
 * @endcode
 *
 * This will output message with bright red that you cannot miss.
 *
 * There are two
 * special and very  self explanatory symbols: <b>underline</b> and <b>blink</b>. Neither of
 * them work on Windows, unfortunately (but you can file a request at Microsoft's).
 * Xterm on linux would display both, but gnome-terminal does not support blink
 * (there is a bug opened on this since 2010).
 *
 * Here is an example on how to write a blinking line and underlined one.
 *
 * @code{.cpp}
 * #include "qlog.hpp"
 * using namespace qlog;
 *
 * int main()
 * {
 *     qlog::error << "This " << underline() << "word" << color() << " should be underlined" << '\n';
 *     qlog::error << "This " << blink() << "word" << color() << " should be blinking" << '\n';
 * }
 * @endcode
 *
 *
 * MULTITHREADING
 * --------------
 *
 * qlog allows multiple threads to run on the same level of logging. When support is enabled, many threads
 * can concurrently write on the same logger level. Pay attention that different level of logging
 * will not be multithread-safe, they should write to different outputs.
 *
 * To enable multithread mode, you can define QLOG_MULTITHREAD and provide a class or a struct called
 * mutex in the qlog namespace. Alternatively, defining QLOG_MULTITHREAD_PTHREAD lets you use pthread
 * mutexes.
 */

#include <ostream>
#ifdef QLOG_USE_ASSERTS
#   include <assert.h>
#   define QLOG_ASSERT(a); assert(a);
#else
#   define QLOG_ASSERT(a);
#endif

#ifdef QLOG_MULTITHREAD_PTHREAD
#   include <pthread.h>
#   ifndef QLOG_MULTITHREAD
#       define QLOG_MULTITHREAD
#   endif
#endif

#ifdef QLOG_MULTITHREAD_CPP11
#   include <mutex>
#   ifndef QLOG_MULTITHREAD
#       define QLOG_MULTITHREAD
#   endif
#endif

// windows color support
#ifdef WIN32
#	include <windows.h>
#endif

#ifdef QLOG_MULTITHREAD_WIN32
#	ifndef QLOG_MULTITHREAD
#		define QLOG_MULTITHREAD
#	endif
#endif

#ifndef QLOG_MAX_DECORATIONS
#   define QLOG_MAX_DECORATIONS 10
#endif

// hide symbols on linux
#if __GNUC__ >= 4
#   pragma GCC visibility push(hidden)
#endif

// let the user defines his own namespace
#ifdef QLOG_NAMESPACE
namespace QLOG_NAMESPACE
{
#else
namespace qlog
{
#endif

// -------------------------------------------------------------------------- //

// mutex
#ifdef QLOG_MULTITHREAD_PTHREAD
struct cannot_create_mutex : std::exception
{
};

struct mutex
{
    mutex()
        :m_attr()
        ,m_mutex()
    {
        if (pthread_mutexattr_init( &m_attr ) != 0)
            throw cannot_create_mutex();

        if (pthread_mutex_init( &m_mutex, &m_attr ) != 0)
            throw cannot_create_mutex();
    }

    ~mutex()
    {
        pthread_mutex_destroy( &m_mutex );
        pthread_mutexattr_destroy( &m_attr );
    }

    bool lock()
    {
        return pthread_mutex_lock( &m_mutex ) == 0;
    }

    bool unlock()
    {
        return pthread_mutex_unlock( &m_mutex ) == 0;
    }

private:
    pthread_mutexattr_t m_attr;
    pthread_mutex_t m_mutex;
};
#elif defined QLOG_MULTITHREAD_WIN32
struct mutex
{
	mutex()
		:m_section()
	{
		InitializeCriticalSection( &m_section );
	}

	~mutex()
	{
		DeleteCriticalSection( &m_section );
	}

	bool lock()
	{
		EnterCriticalSection( &m_section );
		return true;
	}

	bool unlock()
	{
		LeaveCriticalSection( &m_section );
		return true;
	}

private:
	CRITICAL_SECTION m_section;
};
#elif defined QLOG_MULTITHREAD_CPP11
using std::mutex;
#endif

// -------------------------------------------------------------------------- //
// the different levels of logging

namespace loglevel
{
static const unsigned disabled = 10;

static const unsigned debug = 1;
static const unsigned trace = 2;
static const unsigned info = 3;
static const unsigned warning = 4;
static const unsigned error = 5;
}

#ifdef WIN32
/** @todo make it exception-safe */
static inline
void * get_console_function( char * name )
{
    static HMODULE kernel32=( HMODULE )0xffffffff;

    if( kernel32==0 )
        return NULL;

    if( kernel32==( HMODULE )0xffffffff )
    {
        kernel32=LoadLibraryA( "kernel32.dll" );

        if( kernel32==0 )
            return 0;
    }

    return GetProcAddress( kernel32,name );
}

typedef BOOL ( WINAPI * console_function )( HANDLE hConsoleOutput, WORD attr );
#endif

// -------------------------------------------------------------------------- //
// hack to catch std::endl;
typedef std::basic_ostream<char, std::char_traits<char> > cout_type;
typedef cout_type & ( *standard_endline )( cout_type & );

// -------------------------------------------------------------------------- //

/**
 * @cond GENERATE_INTERNAL_DOCUMENTATION
 * @struct user_global_settings
 * @brief This structure is used to store information across the different compile units.
 */
template<typename T>
struct user_global_settings
{
    static unsigned loglevel; ///< The current level of logging
    static bool initialized;

#ifdef WIN32
    static HANDLE console_handle;
    static console_function set_text_attribute;
#endif
};

/** @endcond */
// -------------------------------------------------------------------------- //

/**@private
  *@brief The current level of logging */
template<typename T>
unsigned user_global_settings<T>::loglevel = loglevel::error;

/**@private
  *@brief Whether the library is initialized */
template<typename T>
bool user_global_settings<T>::initialized = false;

#ifdef WIN32
/**@private
  *@brief A handle to the console */
template<typename T>
HANDLE user_global_settings<T>::console_handle = INVALID_HANDLE_VALUE;

/**@private
  *@brief A function pointer a function that will set console attributes */
template<typename T>
console_function user_global_settings<T>::set_text_attribute = 0;
#endif

typedef user_global_settings<int> settings;

#ifdef __GNUC__
#   define QLOG_SUPPRESS_NOT_USED_WARN __attribute__ ((unused))
#else
#   define QLOG_SUPPRESS_NOT_USED_WARN
#endif

static QLOG_SUPPRESS_NOT_USED_WARN void set_loglevel( unsigned );
static QLOG_SUPPRESS_NOT_USED_WARN unsigned get_loglevel();

static inline
void set_loglevel( unsigned level )
{
    settings::loglevel = level;
}

static inline
unsigned get_loglevel()
{
    return settings::loglevel;
}

// -------------------------------------------------------------------------- //
/**@struct decoration
 * @cond GENERATE_INTERNAL_DOCUMENTATION
 */
struct decoration
{
    virtual ~decoration() throw() { }
    virtual void apply( std::ostream & _ostr ) = 0;
};

struct text_decoration : public decoration
{
    explicit
    text_decoration( const char * _txt )
        :m_txt( _txt )
    {
        QLOG_ASSERT( _txt );
    }

    text_decoration( text_decoration & _copy )
        :m_txt( _copy.m_txt )
    {
    }

    text_decoration & operator=( text_decoration & _obj )
    {
        m_txt = _obj.m_txt;
        return *this;
    }

    /**@todo can std::ostream << throw? what does it do if ostr has a bad bit set */
    virtual void apply( std::ostream & _ostr )
    {
        _ostr << m_txt;
    }

    virtual ~text_decoration() throw()
    {
        QLOG_ASSERT( m_txt );
    }

private:
    const char * m_txt;
};

// -------------------------------------------------------------------------- //
template <unsigned loglevel, bool append>
struct decorater
{
    decorater()
    {
    }

    /**@brief Registers a new decoration
     * @param[in] _deco The new decoration to register
     * @warning Only a limited number of decoration can be added (10 by default)
     * @throw nothing */
    void add_decoration( decoration & _deco )
    {
        QLOG_ASSERT( m_last_index < QLOG_MAX_DECORATIONS );
        try
        {
            m_list[m_last_index] = &_deco;
            ++m_last_index;
        }
        catch( const std::bad_alloc & )
        {
            QLOG_ASSERT( 0 && "std::bad_alloc" );
        }
    }

    /**@brief Apply all decorations to an output stream
     * @param[in] _ostr The outstream to which decorations will be applied
     * @throw nothing */
    void apply_all( std::ostream & _ostr )
    {
        for( size_t i = 0; i < m_last_index; ++i )
        {
            m_list[i]->apply( _ostr );
        }
    }

    /**@brief Deletes all the registered decorations
     * @throw nothing */
    void reset()
    {
        for( size_t i = 0; i < m_last_index; ++i )
        {
            delete m_list[i];
        }
        m_last_index = 0;
    }
    static size_t m_last_index;
    static decoration * m_list[QLOG_MAX_DECORATIONS];
};

template< unsigned loglevel, bool append >
size_t decorater<loglevel, append>::m_last_index = 0;

template< unsigned loglevel, bool append >
decoration * decorater<loglevel, append>::m_list[QLOG_MAX_DECORATIONS];
// -------------------------------------------------------------------------- //
template< unsigned loglevel, bool append > inline
decorater<loglevel, append> & operator << ( decorater<loglevel, append> & _dec, const char * _txt )
{
    QLOG_ASSERT( 0 != _txt );

    try
    {
        decoration * const newdeco = new text_decoration( _txt );
        _dec.add_decoration( *newdeco );
    }
    catch( const std::bad_alloc & )
    {
        QLOG_ASSERT( 0 && "std::bad_alloc" );
    }

    return _dec;
}
/**@endcond */

// -------------------------------------------------------------------------- //
/**@struct logger
 * @brief An object that logs messages
 * @tparam level The level of detail the object logs at */
template< unsigned level >
struct logger
{
    /**@brief Constructs a logger
     * @warning This should almost never been used. 5 loggers are already created
     *          by the library in each compile unit to be used by the user.
     * @param[in] _disabled Whether this logger actually writes messages */
    explicit
    logger( bool _disabled = false )
        :m_disabled( _disabled )
        ,m_nbReceivers( 0 )
    {
    }

    /**@brief Copies a logger
     * @warning This should almost never been used. 5 loggers are already created
     *          by the library in each compile unit to be used by the user.
     * @param[in] _logger The logger to copy */
    logger( const logger & _logger )
        :m_disabled( _logger.m_disabled )
        ,m_nbReceivers( _logger.m_nbReceivers )
    {
    }

    ~logger()
    {
        QLOG_ASSERT( 0 == m_nbReceivers );
        reset_decoration();
    }

    /**@brief Removes all prepending or appending text */
    void reset_decoration()
    {
        reset_prepend();
        reset_append();
    }

    /**@brief Removes all appending text */
    void reset_append()
    {
        m_append.reset();
    }

    /**@brief Removes all prepending text */
    void reset_prepend()
    {
        m_prepend.reset();
    }


    /**@brief Assigns a logger
     * @param[in] _copy Another logger
     * @warning Assigning a logger is not permitted
     * @return *this */
    logger & operator=( const logger & _copy );

    /**@brief Checks if the logger is able to log
     * @return True if the logger can log */
    bool isDisabled() const { return m_disabled; }

    /**@brief Checks and outputs a message.
     * @warning This is automatically be called by @c operator<<
     * @param[in] _message The message to check and output
     * @param[in] _first_part Set to true if a custom message should be appended
     *
     * Checking whether the message can be output consists in:
     * - seeing whether the detail level of the logger is sufficient to output
     *   the message: an warning logger cannot output message when the logging
     *   level is set to error.
     * - checking whether an output stream has been set.
     * - checking whether the logger has been disabled during construction
     *
     * @see logger::logger(bool _disabled)
     */
    template< typename T >
    void treat( const T & _message, bool _first_part ) const
    {
        if( can_log() )
        {
            QLOG_ASSERT( m_output );
            if( _first_part )
                m_prepend.apply_all( *m_output );

            ( *m_output ) << _message;
        }
    }

    /**@brief Sets the destination of the messages
     * @param[in] _output A std::ostream object that will receive the next messages
     *
     * Use this function if you want a certain logger to write its message to
     * a customized output stream. The most frequent use is to direct messages
     * to a file.
     *
     * @code{.cpp}
     * #include <fstream>
     * #include <string>
     * #include "qlog.hpp"
     *
     * using namespace qlog;
     *
     * int main()
     * {
     *     const std::string filename("output.log");
     *     std::ofstream output_file( filename.c_str() );
     *
     *     qlog::error.set_output( output_file );
     *
     *     qlog::error << "This message will be written in output.log" << std::endl;
     * }
     * @endcode
     */
    void set_output( std::ostream & _output )
    {
        m_output = &_output;
    }

    /**@brief Adds a custom text after all logged messages. */
    decorater<level, true> & append()
    {
        return m_append;
    }

    /**@brief Adds a custom text before all logged messages. */
    decorater<level, false> & prepend()
    {
        return m_prepend;
    }

    /**@brief Informs the logger that std::endl, or other io manipulators, have been passed
     * @cond GENERATE_INTERNAL_DOCUMENTATION
     * @param[in] _func The io function, such as std::endl
     * @param[in] _first_message Whether it is the first element of the << series.
     * @private */
    void signal( standard_endline _func, bool _first_message = false ) const
    {
        if( can_log() )
        {
            if( _first_message )
            {
#               ifdef QLOG_MULTITHREAD
                lock();
#               endif
                m_prepend.apply_all( *m_output );
            }

            _func( *m_output );
        }
    }

    void signal_creation() const
    {
        ++m_nbReceivers;
    }

    void signal_destruction() const
    {
        QLOG_ASSERT( m_nbReceivers );
        if (0 == --m_nbReceivers)
        {
            signal_end();
#           ifdef QLOG_MULTITHREAD
            unlock();
#           endif
        }
    }

#   ifdef QLOG_MULTITHREAD
    void lock() const
    {
        m_mutex->lock();
    }

    void unlock() const
    {
        m_mutex->unlock();
    }

    bool init_mutex()
    {
        bool ret = false;
        QLOG_ASSERT( 0 == m_mutex );
        try
        {
            m_mutex = new mutex();
            ret = true;
        } catch (...)
        {
            m_mutex = 0;
            ret = false;
        }

        return ret;
    }

    void destroy_mutex()
    {
        QLOG_ASSERT( 0 != m_mutex );
        delete m_mutex;
        m_mutex = 0;
    }
#   endif

    /**@endcond*/

    /**@brief Conditional logging
     * @param[in] _cond Pass true to enable logging
     * @return A disabled logger if true has been passed, an enabled logger otherwise */
    logger operator()( bool _cond )
    {
        logger ret( *this );
        ret.m_disabled = !_cond;
        return ret;
    }

    /**@brief Disables logging (no further message will be output)
     * @throw nothing */
    void disable() { m_disabled = true; }

    /**@brief Resumes logging */
    void enable() { m_disabled = false; }

private:
    bool m_disabled;
    mutable unsigned m_nbReceivers;
    static std::ostream * m_output;
    static decorater<level, false> m_prepend;
    static decorater<level, true> m_append;
#   ifdef QLOG_MULTITHREAD
    static mutex * m_mutex;
#   endif

private:
    /**@brief Helper function to check that the logger can output messages
     * @cond GENERATE_INTERNAL_DOCUMENTATION
     * @throw nothing
     * @private */
    bool can_log() const
    {
        return ( level >= get_loglevel() ) && m_output && !isDisabled();
    }

    /**@brief Informs the logger that the last message was treated and that custom text can be appended
     * @private */
    void signal_end() const
    {
        QLOG_ASSERT( 0 == m_nbReceivers );

        if( can_log() )
            m_append.apply_all( *m_output );
    }
    /** @endcond */
};
// -------------------------------------------------------------------------- //

#ifdef QLOG_MULTITHREAD
template< unsigned level >
mutex * logger<level>::m_mutex = 0;
#endif

template< unsigned level >
std::ostream * logger<level>::m_output = 0;

template< unsigned level >
decorater<level, true> logger<level>::m_append;

template< unsigned level >
decorater<level, false> logger<level>::m_prepend;

// -------------------------------------------------------------------------- //
/**@struct receiver
 * @cond GENERATE_INTERNAL_DOCUMENTATION
 * @brief A light-weight structure that receives a stream.
 *
 * The purpose of receiver is to detect when a series of operator<< ends.
 * Let’s consider this example:
 * logger << "value = " << value;
 *
 * I want to append a custom text so that the written message
 * looks something like "[..] value = 35\n", which means that I have to add
 * "\n" at some point. receiver is a helper structure that will let us know
 * about that.
 *
 * When logger << "value = " is first proceeded, it calls
 * operator<<( const logger &, std::string & ), which returns a temporary receiver
 * object.
 *
 * Then the second part of the message is treated: "<< value". This time, the
 * function operator << (const receiver &, int) is called. It treated the value
 * and returns a new temporary receiver.
 *
 * As there is no other message to treat, this last temporary receiver dies.
 * Its destructor inform the logger object that a custom text should be appended.
 * The logger object is not called from the destructor if any operator<< has been
 * called on the temporary receiver.
 */
template< unsigned level >
struct receiver
{
    explicit
    receiver( const logger<level> * _logger, bool _muted = false )
        :m_logger( _logger )
        ,m_muted( _muted )
    {
        QLOG_ASSERT( 0 != _logger );
        _logger->signal_creation();
    }

    receiver( const receiver & _copy )
        :m_logger( _copy.m_logger )
        ,m_muted( _copy.m_muted )
    {
        m_logger->signal_creation();
    }

    ~receiver()
    {
        QLOG_ASSERT( 0 != m_logger);
        m_logger->signal_destruction();
    }

    bool is_muted() const { return m_muted; }
    void signal( standard_endline _func ) const
    {
        if( !is_muted() )
        {
            m_logger->signal( _func );
        }
    }

    template< typename T >
    receiver treat( const T & _message, bool _first_part ) const
    {
        if( !m_muted )
            m_logger->treat( _message, _first_part );

        return *this;
    }

private:
    receiver operator=( const receiver & );

private:
    const logger<level> * m_logger;
    mutable bool m_muted;
};

// -------------------------------------------------------------------------- //
template< unsigned level,typename T > inline
receiver<level> operator<<( const receiver<level> & _receiver,  const T & _message )
{
    return _receiver.treat( _message, false );
}

// -------------------------------------------------------------------------- //
template< unsigned level > inline
receiver<level> operator<<( const receiver<level> & _recv, standard_endline _func )
{
    _recv.signal( _func );
    return _recv;
}

// -------------------------------------------------------------------------- //
template< unsigned level, typename T > inline
receiver<level> operator << ( const logger<level> & _logger, const T & _message )
{
#   ifdef QLOG_MULTITHREAD
    _logger.lock();
#   endif
    return receiver<level>( &_logger ).treat( _message, true );
}

// -------------------------------------------------------------------------- //
template< unsigned level > inline
receiver<level> operator<<( const logger<level> & _logger, standard_endline _func )
{
    _logger.signal( _func, true );
    return receiver<level>( &_logger );
}
/** @endcond */
// -------------------------------------------------------------------------- //

#ifndef QLOG_NAME_LOGGER_DEBUG
#   define QLOG_NAME_LOGGER_DEBUG debug
#endif
#ifndef QLOG_NAME_LOGGER_TRACE
#   define QLOG_NAME_LOGGER_TRACE trace
#endif
#ifndef QLOG_NAME_LOGGER_INFO
#   define QLOG_NAME_LOGGER_INFO info
#endif
#ifndef QLOG_NAME_LOGGER_WARNING
#   define QLOG_NAME_LOGGER_WARNING warning
#endif
#ifndef QLOG_NAME_LOGGER_ERROR
#   define QLOG_NAME_LOGGER_ERROR error
#endif

static logger<loglevel::debug> QLOG_NAME_LOGGER_DEBUG ;
static logger<loglevel::trace> QLOG_NAME_LOGGER_TRACE ;
static logger<loglevel::info> QLOG_NAME_LOGGER_INFO ;
static logger<loglevel::warning> QLOG_NAME_LOGGER_WARNING ;
static logger<loglevel::error> QLOG_NAME_LOGGER_ERROR ;

static inline
void set_output( std::ostream & _new_output )
{
    QLOG_NAME_LOGGER_DEBUG . set_output( _new_output );
    QLOG_NAME_LOGGER_TRACE . set_output( _new_output );
    QLOG_NAME_LOGGER_INFO . set_output( _new_output );
    QLOG_NAME_LOGGER_WARNING . set_output( _new_output );
    QLOG_NAME_LOGGER_ERROR . set_output( _new_output );
}


// -------------------------------------------------------------------------- //
/**@brief Terminates the library
 * @note This is only useful on Windows */
static inline
void destroy()
{
    QLOG_ASSERT( settings::initialized );

#	ifdef WIN32
    settings::console_handle = 0;
    settings::set_text_attribute = 0;
#   endif

    QLOG_NAME_LOGGER_DEBUG . reset_decoration();
    QLOG_NAME_LOGGER_TRACE . reset_decoration();
    QLOG_NAME_LOGGER_INFO . reset_decoration();
    QLOG_NAME_LOGGER_WARNING . reset_decoration();
    QLOG_NAME_LOGGER_ERROR . reset_decoration();

    QLOG_NAME_LOGGER_DEBUG . disable();
    QLOG_NAME_LOGGER_TRACE . disable();
    QLOG_NAME_LOGGER_INFO . disable();
    QLOG_NAME_LOGGER_WARNING . disable();
    QLOG_NAME_LOGGER_ERROR . disable();

    // resetting decorations
    QLOG_NAME_LOGGER_DEBUG . prepend().reset();
    QLOG_NAME_LOGGER_DEBUG . append().reset();
    QLOG_NAME_LOGGER_TRACE . prepend().reset();
    QLOG_NAME_LOGGER_TRACE . append().reset();
    QLOG_NAME_LOGGER_INFO . prepend().reset();
    QLOG_NAME_LOGGER_INFO . append().reset();
    QLOG_NAME_LOGGER_ERROR . prepend().reset();
    QLOG_NAME_LOGGER_ERROR . append().reset();
    QLOG_NAME_LOGGER_WARNING . prepend().reset();
    QLOG_NAME_LOGGER_WARNING . append().reset();

#   ifdef QLOG_MULTITHREAD
    QLOG_NAME_LOGGER_DEBUG . destroy_mutex();
    QLOG_NAME_LOGGER_TRACE . destroy_mutex();
    QLOG_NAME_LOGGER_INFO . destroy_mutex();
    QLOG_NAME_LOGGER_WARNING . destroy_mutex();
    QLOG_NAME_LOGGER_ERROR . destroy_mutex();
#   endif

    settings::initialized = false;
}
// -------------------------------------------------------------------------- //
/**@brief Initializes the library.
 * @return true If the library in correctly initialized. If false is returned, then you should call
 *         destroy and not use the library.
 * @warning This should be called only once. */
static inline
bool init()
{
    QLOG_ASSERT( !settings::initialized );

#   ifdef WIN32
    settings::console_handle = GetStdHandle( STD_OUTPUT_HANDLE );
    settings::set_text_attribute = static_cast<console_function>( get_console_function( "SetConsoleTextAttribute" ) );
#   endif

    QLOG_NAME_LOGGER_DEBUG . enable();
    QLOG_NAME_LOGGER_TRACE . enable();
    QLOG_NAME_LOGGER_INFO . enable();
    QLOG_NAME_LOGGER_WARNING . enable();
    QLOG_NAME_LOGGER_ERROR . enable();

    bool init = true;

#   ifdef QLOG_MULTITHREAD
    init = QLOG_NAME_LOGGER_DEBUG . init_mutex();

    if ( init )
        init = QLOG_NAME_LOGGER_TRACE . init_mutex();

    if ( init )
        init = QLOG_NAME_LOGGER_INFO . init_mutex();

    if ( init )
        init = QLOG_NAME_LOGGER_WARNING . init_mutex();

    if ( init )
        init = QLOG_NAME_LOGGER_ERROR . init_mutex();
#   endif

    settings::initialized = init;

    return settings::initialized;
}

static const unsigned black = 1;
static const unsigned red = 2;
static const unsigned green = 3;
static const unsigned yellow = 4;
static const unsigned blue = 5;
static const unsigned magenta = 6;
static const unsigned cyan = 7;
static const unsigned white = 8;
static const unsigned gray = 9;

// -------------------------------------------------------------------------- //
/**@struct underline
 * @brief A manipulator to underline text
 * @note Underlining is ignored on windows
 *
 * An underline object can be used to tell the console that the following text
 * should be underlined. Use @b color() to disable underlining. This example
 * shows a simple underlined word.
 *
 * @code{.cpp}
 * #include <iostream>
 * #include "qlog.hpp"
 * using namespace qlog;
 *
 * int main()
 * {
 *     qlog::set_output( std::cout );
 *     qlog::error << "This " << underline() << "word" << color() << " should be underlined" << '\n';
 * }
 * @endcode
 * This will output the following:
 * @image html underline.png
 *
 */
struct underline
{
};

// -------------------------------------------------------------------------- //
/**@struct blink
 * @brief A manipulator to make text blink
 * @note Blinking is ignored on windows, and not supported on all terminals
 *
 * An underline object can be used to tell the console that the following text
 * should be blinking. Use @b color() to disable blinking. This example
 * shows a simple blinking word.
 *
 * @code{.cpp}
 * #include <iostream>
 * #include "qlog.hpp"
 * using namespace qlog;
 *
 * int main()
 * {
 *     qlog::set_output( std::cout );
 *     qlog::error << "This " << blink() << "word" << color() << " should be underlined" << '\n';
 * }
 * @endcode
 *
 * This will make the word blink.
 *
 */
struct blink
{
};


#ifndef WIN32
struct color
{
    virtual ~color()
    {
        QLOG_ASSERT( 0 != m_foreground );
        QLOG_ASSERT( 0 != m_background );
        QLOG_ASSERT( 0 != m_bold );
    }

    explicit
    color( bool _bold = false )
        :m_foreground( "\033[0m" )
        ,m_background( "" )
        ,m_bold( _bold ? "\033[1m" : "" )
    {
    }

    color( const color & _copy )
        :m_foreground( _copy.m_foreground )
        ,m_background( _copy.m_background )
        ,m_bold( _copy.m_bold )
    {
    }

    color & operator=( const color & _copy )
    {
        m_foreground =_copy.m_foreground;
        m_background = _copy.m_background;
        m_bold = _copy.m_bold;

        return *this;
    }
    explicit
    color( unsigned _foreground, bool _bold = false )
        :m_foreground( "\033[0m" )
        ,m_background( "" )
        ,m_bold( _bold ? "\033[1m" : "" )
    {
        switch( _foreground )
        {
        case black: m_foreground = "\033[30m"; break;
        case red: m_foreground = "\033[31m"; break;
        case green: m_foreground = "\033[32m"; break;
        case yellow: m_foreground = "\033[33m"; break;
        case blue: m_foreground = "\033[34m"; break;
        case magenta: m_foreground = "\033[35m"; break;
        case cyan: m_foreground = "\033[36m"; break;
        case white: m_foreground = "\033[37m"; break;

        default: break;
        }
    }

    color( unsigned _foreground, unsigned _background, bool _bold = false )
        :m_foreground( "" )
        ,m_background( "" )
        ,m_bold( _bold ? "\033[1m" : "" )
    {
        switch( _foreground )
        {
        case black: m_foreground = "\033[30;"; break;
        case red: m_foreground = "\033[31;"; break;
        case green: m_foreground = "\033[32;"; break;
        case yellow: m_foreground = "\033[33;"; break;
        case blue: m_foreground = "\033[34;"; break;
        case magenta: m_foreground = "\033[35;"; break;
        case cyan: m_foreground = "\033[36;"; break;
        case white: m_foreground = "\033[37;"; break;

        default: break;
        }

        switch( _background )
        {
        case black: m_background = "40m"; break;
        case red: m_background = "41m"; break;
        case green: m_background = "42m"; break;
        case yellow: m_background = "43m"; break;
        case blue: m_background = "44m"; break;
        case magenta: m_background = "45m"; break;
        case cyan: m_background = "46m"; break;
        case white: m_background = "47m"; break;

        default: break;
        }

        QLOG_ASSERT(0 != m_foreground);
        QLOG_ASSERT(0 != m_background);
        QLOG_ASSERT(0 != m_bold);
    }

    const char * getBold() const { return m_bold; }
    const char * getBackground() const { return m_background; }
    const char * getForeground() const { return m_foreground; }

private:
    const char * m_foreground;
    const char * m_background;
    const char * m_bold;
};

template<unsigned level> inline
receiver<level> operator <<( const logger<level> & _logger, const color & _color )
{
    return _logger << _color.getForeground() << _color.getBackground() << _color.getBold();
}

template<unsigned level> inline
receiver<level> operator <<( const receiver<level> & _recv, const color & _color )
{
    return _recv << _color.getForeground() << _color.getBackground() << _color.getBold();
}

// -------------------------------------------------------------------------- //
template<unsigned level> inline
receiver<level> operator <<( const logger<level> & _logger, const underline & )
{
    return _logger << "\033[4m";
}

template<unsigned level> inline
receiver<level> operator <<( const receiver<level> & _recv, const underline & )
{
    return _recv << "\033[4m";
}

// -------------------------------------------------------------------------- //
template<unsigned level> inline
receiver<level> operator <<( const logger<level> & _logger, const blink & )
{
    return _logger << "\033[5m";
}

template<unsigned level> inline
receiver<level> operator <<( const receiver<level> & _recv, const blink & )
{
    return _recv << "\033[5m";
}

#else // WIN32

struct color
{
    explicit
    color( bool _bold = false )
        :m_attributes( 0 )
    {
        setForeground( white, _bold );
        setBackground( black );
    }

    virtual ~color()
    {
    }

    explicit
    color( unsigned _foreground, bool _bold = false )
        :m_attributes( 0 )
    {
        setForeground( _foreground, _bold );
    }

    color( unsigned _foreground, unsigned _background, bool _bold = false )
        :m_attributes( 0 )
    {
        setForeground( _foreground, _bold );
        setBackground( _background );
    }

    WORD getAttributes() const { return m_attributes; }

private:
    void setForeground( unsigned _color, bool _bold )
    {
        switch( _color )
        {
        case black: m_attributes = 0; break;
        case blue:	m_attributes = FOREGROUND_BLUE; break;
        case green: m_attributes = FOREGROUND_GREEN; break;
        case red:	m_attributes = FOREGROUND_RED; break;
        case yellow: m_attributes = FOREGROUND_GREEN | FOREGROUND_RED; break;
        case gray: m_attributes = FOREGROUND_INTENSITY; break;
        case magenta: m_attributes = FOREGROUND_BLUE | FOREGROUND_RED; break;
        default:
        case white: m_attributes = FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN; break;
        }

        if( _bold )
            m_attributes |= FOREGROUND_INTENSITY;
    }

    void setBackground( unsigned _color )
    {
        WORD bgAttributes = 0;

        switch( _color )
        {
        case white: bgAttributes |= BACKGROUND_BLUE | BACKGROUND_RED | BACKGROUND_GREEN;
        case blue:	bgAttributes |= BACKGROUND_BLUE; break;
        case red:	bgAttributes |= BACKGROUND_RED; break;
        case green: bgAttributes |= BACKGROUND_GREEN; break;
        case gray:	bgAttributes |= BACKGROUND_INTENSITY; break;
        case yellow: bgAttributes |= BACKGROUND_GREEN | BACKGROUND_RED; break;
        case magenta: bgAttributes |= BACKGROUND_BLUE | BACKGROUND_RED; break;

        default:
        case black: bgAttributes |= 0; break;
        }

        m_attributes |= bgAttributes;
    }
    WORD m_attributes;
};

template<unsigned level> inline
receiver<level> operator <<( const logger<level> & _logger, const color & _color )
{
    QLOG_ASSERT( settings::set_text_attribute && settings::console_handle );
    settings::set_text_attribute( settings::console_handle, _color.getAttributes() );
    return receiver<level>( &_logger ).treat( "", true );
}

template<unsigned level> inline
receiver<level> operator <<( const receiver<level> & _recv, const color & _color )
{
    QLOG_ASSERT( settings::set_text_attribute && settings::console_handle );
    settings::set_text_attribute( settings::console_handle, _color.getAttributes() );
    return _recv.treat( "", false );
}

// -------------------------------------------------------------------------- //
template<unsigned level> inline
receiver<level> operator <<( const logger<level> & _logger, const underline & )
{
    return receiver<level>( &_logger ).treat( "", true );
}

template<unsigned level> inline
receiver<level> operator <<( const receiver<level> & _recv, const underline & )
{
    return _recv.treat( "", false );
}
// -------------------------------------------------------------------------- //
template<unsigned level> inline
receiver<level> operator <<( const logger<level> & _logger, const blink & )
{
    return receiver<level>( &_logger ).treat( "", true );
}

template<unsigned level> inline
receiver<level> operator <<( const receiver<level> & _recv, const blink & )
{
    return _recv.treat( "", false );
}

#endif

// -------------------------------------------------------------------------- //
/**@struct color_decoration
 * @cond GENERATE_INTERNAL_DOCUMENTATION
 */
struct color_decoration : public decoration
{
    color_decoration( const color & _color )
        :m_color( _color )
    {
    }

    virtual void apply( std::ostream & _ostr )
    {
#       ifndef WIN32
        _ostr << m_color.getForeground() << m_color.getBackground() << m_color.getBold();
#       else
        settings::set_text_attribute( settings::console_handle, m_color.getAttributes() );
#       endif
    }
    virtual ~color_decoration() throw() { }

private:
    color m_color;
};

// -------------------------------------------------------------------------- //
template <unsigned level, bool append >
decorater<level, append> & operator<< ( decorater<level, append> & _dec, const color & _color )
{
    try
    {
        decoration * const newdeco = new color_decoration( _color );
        _dec.add_decoration( *newdeco );
    }
    catch( const std::bad_alloc & )
    {
        QLOG_ASSERT( 0 && "std::bad_alloc" );
    }

    return _dec;
}
/**@endcond */

// -------------------------------------------------------------------------- //
struct blink_decoration : public decoration
{
    virtual ~blink_decoration() throw() { }
    virtual void apply( std::ostream & _ostr )
    {

#       ifndef WIN32
        _ostr << "\033[5m";
#       else
        ( void )_ostr;
#       endif
    }
};

// -------------------------------------------------------------------------- //
template<unsigned level, bool append > inline
decorater<level, append> & operator << ( decorater<level, append> & _dec, const blink & )
{
    try
    {
        decoration * const newdeco = new blink_decoration();
        _dec.add_decoration( *newdeco );
    }
    catch( const std::bad_alloc & )
    {
        QLOG_ASSERT( 0 && "std::bad_alloc" );
    }

    return _dec;
}

// -------------------------------------------------------------------------- //
struct underline_decoration : public decoration
{
    virtual ~underline_decoration() throw()  { }
    virtual void apply( std::ostream & _ostr )
    {

#       ifndef WIN32
        _ostr << "\033[4m";;
#       else
        ( void )_ostr;
#       endif
    }
};

// -------------------------------------------------------------------------- //
template< unsigned loglevel, bool append > inline
decorater<loglevel, append> & operator << ( decorater<loglevel, append> & _dec, const underline & )
{
    try
    {
        decoration * const newdeco = new underline_decoration();
        _dec.add_decoration( *newdeco );
    }
    catch( const std::bad_alloc & )
    {
        QLOG_ASSERT( 0 && "std::bad_alloc" );
    }

    return _dec;
}

} // namespace
#if __GNUC__ >= 4
#   pragma GCC visibility pop
#endif

#endif // QLOG_HPP
