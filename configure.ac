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

# Checks for programs.
AC_PROG_CXX
AC_PROG_INSTALL

# AC_PROG_CXX doesn't let us determine whether a CXX compiler is available
AC_CHECK_PROG([CXX_COMPILER],[$CXX],[yes],[no])
if test "x$CXX_COMPILER" = "xno"
then
	AC_MSG_ERROR([Unable to find a C++ compiler.])
fi


# Debug mode
AC_ARG_ENABLE([limit-mem],
  AS_HELP_STRING([--enable-limit-mem],[Provides an option to limitate the virtual memory (--limit-mem).]),
  [limit_mem=${enableval}],
  [limit_mem=no])
AC_ARG_ENABLE([debug],
  AS_HELP_STRING([--enable-debug],[Adds asserts and debug code to the build.]),
  [debug_mode=${enableval}],
  [debug_mode=no])
AC_ARG_ENABLE([download],
  AS_HELP_STRING([--enable-download],[Uses curl to download tatoeba material.]),
  [download_mode=${enableval}],
  [download_mode=no])
AC_ARG_ENABLE([python],
  AS_HELP_STRING([--enable-python],[Adds python wrappers.]),
  [python_mode=${enableval}],
  [python_mode=no])
AC_ARG_ENABLE([android],
  AS_HELP_STRING([--enable-android],[Enables compiling android source code.]),
  [android_mode=${enableval}],
  [android_mode=no])
AC_ARG_ENABLE([emulator],
  AS_HELP_STRING([--enable-emulator],[Lets you run the APK in an emulator.]),
  [emulator_mode=${enableval}],
  [emulator_mode=no])
AC_ARG_WITH([android-boost-lib], 
  AS_HELP_STRING([--with-android-boost-lib=/path/to/boost/lib],[Specify the path to boost libraries for android.]),
  [android_boost_lib_path="$withval"])
AC_ARG_WITH([android-boost-include],
  AS_HELP_STRING([--with-android-boost-include=/path/to/boost/include],[Specify the path to boost includes for android.]),
  [android_boost_include_path="$withval"])
AC_ARG_WITH([android-boost-version],
  AS_HELP_STRING([--with-android-boost-version=1.49],[Specify the version of boost for android.]),
  [android_boost_version="$withval"])
AC_ARG_WITH([bootstrap-classpath],
  AS_HELP_STRING([--with-bootstrap-classpath=/path/to/rt.jar],[Specify the bootstrap class path to use.]),
  [android_bootstrap_classpath="$withval"])

if test "$debug_mode" = "no"
then
  CXXFLAGS="$CXXFLAGS -DNDEBUG"
fi

if test "$python_mode" = "yes"
then
  CXXFLAGS="$CXXFLAGS -DUSE_PYTHON_WRAPPER"
fi	

if test "x$android_mode" = "xyes"
then
	AC_PROG_JAVAC
	android_boost_CPPFLAGS="-I $android_boost_include_path"
	android_boost_LIBS="-L $android_boost_lib_path"
	android_boost_version=${android_boost_version/./_}
	AC_SUBST([android_boost_CPPFLAGS])
	AC_SUBST([android_boost_LIBS])
	AC_SUBST([android_boost_version])
	AC_SUBST([android_bootstrap_classpath])

	ANDROID_PATH_PROG_ANDROID([],[AC_MSG_ERROR([Unable to find a program called android, this should come with the android sdk.])])
	ANDROID_PATH_PROG_DX([],[AC_MSG_ERROR([Unable to find dx, this should come with the android sdk.])])
	ANDROID_PATH_PROG_KEYTOOL([],[AC_MSG_ERROR([Unable to find keytool, this should come with the android sdk.])])
	ANDROID_PATH_PROG_AAPT([],[AC_MSG_ERROR([Unable to find aapt, this should come with the android sdk.])])
	ANDROID_PATH_PROG_JARSIGNER([],[AC_MSG_ERROR([Unable to find jarsigner, this should come with the android sdk.])])
	ANDROID_PATH_PROG_ZIPALIGN([],[AC_MSG_ERROR([Unable to find zipalign, this should come with the android sdk.])])
	ANDROID_PATH_PROG_NDK_BUILD([],[AC_MSG_ERROR([Unable to find ndk-build, this should come with the android ndk.])])

	AC_SUBST([TATOPARSER_TARGET_ID],[android-17])

	if test "x$emulator_mode" = "xyes"
	then
		ANDROID_PATH_PROG_EMULATOR([],[AC_MSG_ERROR([Unable to find emulator, this should come with the android sdk.])])
		ANDROID_PATH_PROG_ADB([],[AC_MSG_ERROR([Unable to find adb, this should come with the android sdk.])])
	fi
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
