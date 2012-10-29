#ifndef PREC_LIBRARY_H
#define PREC_LIBRARY_H

// ___________________________ AUTOCONF ________________________________________

#if HAVE_CONFIG_H
#   include <config.h>
#endif

// ___________________________ SYSTEM INCLUDES _________________________________

#include <string.h>
#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <cstdint>
#include <valgrind/valgrind.h>
#include <assert.h>
#include <new>
#include <sstream>
#include <assert.h>
#if HAVE_BOOST_CONFIG_HPP == 1
#   include <boost/config.hpp>
#endif
// ___________________________ COMPATIBILITY ___________________________________


#ifndef BOOST_NO_NOEXCEPT
#   define TATO_NO_THROW noexcept
#else
#   define TATO_NO_THROW throw()
#endif

/**@TODO find a way to know whether the compiler supports "override" */
#define TATO_OVERRIDE

#if defined __llvm__ || defined __clang__
#   define TATO_RESTRICT
#else
#   define TATO_RESTRICT __restrict
#endif
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

// __________________________ LOGGING __________________________________________

#define QDIILOG_NAMESPACE llog
#define QDIILOG_NAME_LOGGER_DEBUG debug
#define QDIILOG_NAME_LOGGER_TRACE trace
#define QDIILOG_NAME_LOGGER_INFO info
#define QDIILOG_NAME_LOGGER_WARNING warning
#define QDIILOG_NAME_LOGGER_ERROR error
#if HAVE_BOOST_CONFIG_HPP == 1
#   define QDIILOG_WITHOUT_BOOST
#endif
#include "qdiilog.hpp"

#endif // PREC_H
