#ifndef PREC_H
#define PREC_H

// ___________________________ AUTOCONF ________________________________________

#if HAVE_CONFIG_H
#   include <config.h>

#   if HAVE_FCNTL_H == 0
#       error "The project cannot be build without <fcntl.h>"
#   endif

#   if HAVE_MEMORY_H == 0 || HAVE_MMAP == 0 || HAVE_MUNMAP == 0
#       error "The project cannot be build without <memory.h> or mmap/munmap calls"
#   endif

#endif

// ___________________________ SYSTEM INCLUDES _________________________________

#include <string.h>
#include <vector>
#include <string>
#include <algorithm>
#include <boost/regex.hpp>
#include <boost/regex/icu.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/cmdline.hpp>
#include <boost/program_options/parsers.hpp>
#include <iostream>
#include <stdexcept>
#include <cstdint>

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

// ______________________________ VALGRIND ___________________________________

#ifdef USE_VALGRIND
#   include <valgrind/valgrind.h>
#endif

// --------------------------- PROJECT INCLUDES --------------------------------

#include "sentence.h"
#include "filter.h"
#include "file_mapper.h"

#endif // PREC_H
