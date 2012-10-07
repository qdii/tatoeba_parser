#ifndef PREC_H
#define PREC_H

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

// ___________________________ COMPATIBILITY ___________________________________

#if __GNUC__ == 4 && (__GNUC_MINOR__ > 7 || \
      (__GNUC_MINOR__ == 7 && __GNUC_PATCHLEVEL__ > 1))
#   define TATO_OVERRIDE override
#   define TATO_NO_THROW nothrow
#else
#   define TATO_OVERRIDE
#   define TATO_NO_THROW throw()
#endif

#if defined __llvm__ || defined __clang__
#   define TATO_RESTRICT
#else
#   define TATO_RESTRICT __restrict__
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

#define QDIILOG_NAMESPACE qlog
#define QDIILOG_NAME_LOGGER_DEBUG debug
#define QDIILOG_NAME_LOGGER_TRACE trace
#define QDIILOG_NAME_LOGGER_INFO info
#define QDIILOG_NAME_LOGGER_WARNING warning
#define QDIILOG_NAME_LOGGER_ERROR error
#include "qdiilog.hpp"

// --------------------------- PROJECT INCLUDES --------------------------------

#include "sentence.h"
#include "filter.h"
#include "file_mapper.h"

#endif // PREC_H
