#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([tatoparser], 3.0, [victor.lavaud@gmail.com])
AM_INIT_AUTOMAKE
AC_LANG(C++)
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADERS([config.h])

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
AC_ARG_ENABLE([android],
  [  --enable-android   compiles android source code.],
  [android_mode=${enableval}],
  [android_mode=no])

if test "$debug_mode" = "no"
then
  CXXFLAGS="$CXXFLAGS -DNDEBUG"
fi

if test "$python_mode" = "yes"
then
  CXXFLAGS="$CXXFLAGS -DUSE_PYTHON_WRAPPER"
fi	

# Checks for programs.
AC_PROG_CXX
AC_PROG_INSTALL

if test "x$android_mode" = "xyes"
then
	AC_PROG_JAVAC

	AC_PATH_PROG([ANDROID],[android],[notfound])
	if test "x$ANDROID" = "xnotfound"
	then
		AC_MSG_ERROR([Unable to find a program called android, this should come with the android sdk.])
	else
		android_sdk_path_=$(dirname "$ANDROID")/..
		AC_SUBST([ANDROID_SDK_PATH],[$android_sdk_path_])
	fi

	AC_PATH_PROG([DX],[dx],[notfound])
	if test "x$DX" = "xnotfound"
	then
		AC_MSG_ERROR([Unable to find dx, this should come with the android sdk.])
	fi

	AC_PATH_PROG([KEYTOOL],[keytool],[notfound])
	if test "x$KEYTOOL" = "xnotfound"
	then
		AC_MSG_ERROR([Unable to find keytool, this should come with the android sdk.])
	fi
	AC_SUBST([TATOPARSER_TARGET_ID],[android-17])
fi

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
		 src/java/Makefile
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
