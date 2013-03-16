#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([tatoparser], 3.0, [victor.lavaud@gmail.com])
AM_INIT_AUTOMAKE
AC_LANG(C++)
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

AC_SUBST([TATOPARSER_SO_VERSION], [3:0:0])
AC_SUBST([TATOPARSER_API_VERSION], [3.0])

LT_PREREQ([2.2])
LT_INIT()

BOOST_REQUIRE

# Debug mode
AC_ARG_ENABLE([limit-mem],
  [  --enable-limit-mem    	provides an option to limitate the virtual memory (--limit-mem).],
  [limit_mem=${enableval}],
  [limit_mem=no])
AC_ARG_ENABLE([debug],
  [  --enable-debug	adds asserts and debug code to the build.],
  [debug_mode=${enableval}],
  [debug_mode=no])
AC_ARG_ENABLE([download],
  [  --enable-download  uses curl to download tatoeba material.],
  [download_mode=${enableval}],
  [download_mode=no])
AC_ARG_ENABLE([python],
  [  --enable-python    adds python wrappers.],
  [python_mode=${enableval}],
  [python_mode=no])

if test "$debug_mode" = "no"
then
  CXXFLAGS="$CXXFLAGS -DNDEBUG"
fi

if test "$python_mode" = "yes"
then
  AC_PATH_PROG([PYTHON_CONFIG],[python-config],[no])
  AS_IF([test "x$PYTHON_CONFIG" = "xno"], [
	AC_MSG_ERROR([Unable to find python-config in PATH.])
  ])

  MY_PYTHON_VERSION=$("$PYTHON_CONFIG" --libs | grep -o '[[23]]\.[[0-9]]')
  MY_BOOST_PYTHON="boost_python-$MY_PYTHON_VERSION"

  CFLAGS_PYTHON_=$("$PYTHON_CONFIG" --cflags)
  LDFLAGS_PYTHON_=$("$PYTHON_CONFIG" --ldflags)
  LIBS_PYTHON_=$("$PYTHON_CONFIG" --libs)
  INCLUDE_PYTHON_=$("$PYTHON_CONFIG" --includes)
  CPPFLAGS_PYTHON_="-DUSE_PYTHON_WRAPPER"

  AC_SUBST([CFLAGS_PYTHON], [$CFLAGS_PYTHON_])
  AC_SUBST([LDFLAGS_PYTHON], ["$LDFLAGS_PYTHON_ -l$MY_BOOST_PYTHON"])
  AC_SUBST([LIBS_PYTHON], [$LIBS_PYTHON_])
  AC_SUBST([INCLUDE_PYTHON], [$INCLUDE_PYTHON_])
  AC_SUBST([CPPFLAGS_PYTHON], [$CPPFLAGS_PYTHON_])
fi	

# Checks for programs.
AC_PROG_CXX
AC_PROG_INSTALL

# Checks for libraries.
AS_IF([test "x$download_mode" = "xyes"], [
  PKG_CHECK_MODULES([CURL], [libcurl])
])

# ICU
PKG_CHECK_MODULES([ICUUC], [icu-uc])
LDFLAGS="$LDFLAGS $ICUUC_LIBS $CURL_LIBS"

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h string.h unistd.h])

# Checks for boost headers
BOOST_REGEX
BOOST_PROGRAM_OPTIONS
AC_CHECK_HEADERS([boost/spirit/include/qi.hpp], , [AC_MSG_ERROR([Unable to find Boost Spirit header (boost/spirit/include/qi.hpp)])])
AC_CHECK_HEADERS([boost/config.hpp], , )
if test "$download_mode" = "yes"
then
  AC_CHECK_HEADERS([curl/curl.h], , [AC_MSG_ERROR([Unable to find curl header (curl/curl.h)])])
fi

# Checks for memory limitation headers
if test "x${limit_mem}" = xyes; then
  AC_CHECK_HEADERS([sys/resource.h], ,[AC_MSG_ERROR([Unable to find <sys/resource.h>, necessary to build debug functions.])])
fi

# Checks for mmap
AC_CHECK_HEADERS([sys/mman.h], ,[AC_MSG_WARN([mman.h was not found on your computer. Parsing will be slower.])])

# Checks for typedefs, structures, and compiler characteristics.
#AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT32_T
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T

# Checks for library functions.
AC_CONFIG_FILES([Makefile
                 src/Makefile
		 include/Makefile
		 unittests/Makefile])
AC_OUTPUT

echo \
"-------------------------------------------------

 ${PACKAGE_NAME} Version ${PACKAGE_VERSION}

 Prefix: '${prefix}'.
 Compiler: '${CXX} ${CXXFLAGS}'

 Now type 'make @<:@<target>@:>@'
   where the optional <target> is:
     all                - build all binaries
     install            - install everything
     check		- run unit tests

--------------------------------------------------"
