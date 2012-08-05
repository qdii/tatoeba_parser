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

NAMESPACE_END

#include "output.h"

#endif //PREC_H
