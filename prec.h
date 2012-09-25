#ifndef PREC_H
#define PREC_H

// ___________________________ NAMESPACE _______________________________________

#ifdef NAMESPACE
#   define NAMESPACE_START namespace NAMESPACE {
#   define NAMESPACE_END }
#   define USING_NAMESPACE using namespace NAMESPACE;
#else
#   define NAMESPACE_START
#   define NAMESPACE_END
#   define USING_NAMESPACE
#endif

// __________________________ LOGGING ________________________________________

#define QDIILOG_NAMESPACE qlog
#define QDIILOG_NAME_LOGGER_DEBUG debug
#define QDIILOG_NAME_LOGGER_TRACE trace
#define QDIILOG_NAME_LOGGER_INFO info
#define QDIILOG_NAME_LOGGER_WARNING warning
#define QDIILOG_NAME_LOGGER_ERROR error
#include "qdiilog.hpp"

// __________________________ERROR CODES________________________________________

NAMESPACE_START
static const int    SUCCESS             = 0;
static const int    CANT_OPEN_FILE      = -1;
static const int    INVALID_ARG         = -2;
static const int    OUT_OF_MEMORY       = -3;
static const int    DOES_NOT_MATCH      = -4;
static const int    INTERNAL_ERROR      = -5;
static const int    CANT_OPEN_SENTENCES_CSV     = -6;
static const int    CANT_OPEN_LINKS_CSV         = -7;
static const int    NO_SUCH_TAG         = -8;
static const int    CANT_OPEN_TAGS_CSV  = -9;
NAMESPACE_END

// __________________________ HANDY METRICS ____________________________________

#include <chrono> 
#define TIME_ME TimeThisFunction<decltype(qlog::info)> timeMe(qlog::info, __func__);
#define TIME_ME_COUT TimeThisFunction<decltype(std::cout)> timeMe(std::cout, __func__);
template<typename T>
struct TimeThisFunction
{
    TimeThisFunction(T & _output, const std::string & _func)
        :m_start(std::chrono::high_resolution_clock::now())
        ,m_output(_output)
        ,m_functionName( _func )
    {
    }

    ~TimeThisFunction()
    {
        using namespace std::chrono;
        const time_point<high_resolution_clock> finish =
            high_resolution_clock::now();

        const duration<double> timeSpent =
            duration_cast<duration<double>>(finish - m_start);

        m_output << m_functionName << " lasted "
                 << timeSpent.count() << " seconds.\n";
    }

private:
    const std::chrono::time_point<std::chrono::high_resolution_clock> m_start;
    T & m_output;
    std::string m_functionName;
};

// ______________________________ VALGRIND ___________________________________

#ifdef USE_VALGRIND
#   include <valgrind/valgrind.h>
#endif

#endif // PREC_H
