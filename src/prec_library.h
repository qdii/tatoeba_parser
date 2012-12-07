#ifndef PREC_LIBRARY_H
#define PREC_LIBRARY_H

// ___________________________ AUTOCONF ________________________________________

#if HAVE_CONFIG_H
#   include <config.h>
#	ifndef BOOST_NO_CXX11_DELETED_FUNCTIONS
#		define TATO_USE_DELETE
#	endif
#	ifndef BOOST_NO_CXX11_NOEXCEPT
#		define TATO_USE_NOEXCEPT
#	endif
#	ifndef BOOST_NO_CXX11_VARIADIC_TEMPLATES
#		define TATO_USE_VARIADIC_TEMPLATES
#	endif
#endif

#ifdef TATE_USE_DELETE
#	define TATO_DELETE = delete
#else
#	define TATO_DELETE
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
#include "tatoparser/namespace.h"

// ___________________________ COMPATIBILITY ___________________________________

#ifdef TATO_USE_NOEXCEPT
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

// __________________________ LOGGING __________________________________________

#define QLOG_USE_ASSERTS
#define QLOG_NAMESPACE llog
#include "qlog.hpp"

#endif // PREC_H
