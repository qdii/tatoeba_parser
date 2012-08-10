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

//_________________________ COMMON INCLUDES __________________________________

#include <cstdint>
#include <assert.h>

NAMESPACE_START
// ___________________________ TYPEDEFS ______________________________________

typedef int_fast32_t        ux;
typedef uint_fast32_t       ix;

// ____________________________ DEBUG  _______________________________________

#ifndef NDEBUG
#   define ASSERT(X) assert(X)
#   define ASSERT_EQ(X, Y) assert((X) == (Y))
#   define VERIFY(X) ASSERT(X)
#   define VERIFY_EQ(X, Y) ASSERT_EQ(X,Y)
#else
#   define ASSERT(X)
#   define ASSERT_EQ(X,Y) ASSERT(X)
#   define VERIFY(X) (X)
#   define VERIFY_EQ(X,Y)  (X)
#endif

// _________________________ ERROR CODES  ____________________________________

static const int    SUCCESS             = 0;
static const int    CANT_OPEN_FILE      = -1;
static const int    INVALID_ARG         = -2;
static const int    OUT_OF_MEMORY       = -3;
static const int    DOES_NOT_MATCH      = -4;
static const int    INTERNAL_ERROR      = -5;
static const int    CANT_OPEN_SENTENCES_CSV     = -6;
static const int    CANT_OPEN_LINKS_CSV         = -7;

NAMESPACE_END

#include "output.h"

#endif //PREC_H
