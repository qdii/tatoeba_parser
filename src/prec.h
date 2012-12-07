#ifndef TATO_PREC_H
#define TATO_PREC_H

// ___________________________ AUTOCONF ________________________________________

#if HAVE_CONFIG_H
#   include <config.h>
#endif

// ___________________________ SYSTEM INCLUDES _________________________________

#include <string.h>
#include <vector>
#include <string>
#include <algorithm>
#include <boost/regex.hpp>
#include <boost/regex/icu.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/cmdline.hpp>
#include <boost/program_options/parsers.hpp>
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
#	ifndef BOOST_NO_CXX11_DELETED_FUNCTIONS
#		define TATO_USE_DELETE
#	endif
#endif
// ___________________________ COMPATIBILITY ___________________________________


#ifndef BOOST_NO_NOEXCEPT
#   define TATO_NO_THROW noexcept
#else
#   define TATO_NO_THROW throw()
#endif

#ifdef TATE_USE_DELETE
#	define TATO_DELETE = delete
#else
#	define TATO_DELETE
#endif

#ifdef __GNUC__
#	define TATO_LIKELY(condition) (__builtin_expect(condition, true))
#	define TATO_UNLIKELY(condition) (__builtin_expect(condition, false))
#else
#	define TATO_LIKELY(condition) (condition)
#	define TATO_UNLIKELY(condition) (condition)
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

#if HAVE_BOOST_CONFIG_HPP == 1
#   define QDIILOG_WITHOUT_BOOST
#endif

#define QLOG_USE_ASSERTS
#include "qlog.hpp"

#endif // TATO_PREC_H
