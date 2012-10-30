#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([tatoparser], 2.5, [victor.lavaud@gmail.com])
AM_INIT_AUTOMAKE
AC_LANG(C++)
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADERS([config.h])

AC_SUBST([TATOPARSER_SO_VERSION], [2:5:0])
AC_SUBST([TATOPARSER_API_VERSION], [2.5])

LT_PREREQ([2.2])
LT_INIT()

# Debug mode
AC_ARG_ENABLE([limit-mem],
  [  --enable-limit-mem    	provides an option to limitate the virtual memory (--limit-mem).],
  [limit_mem=${enableval}],
  [limit_mem=no])
AC_ARG_ENABLE([debug],
  [  --enable-debug	adds asserts and debug code to the build.],
  [debug_mode=${enableval}],
  [debug_mode=no])

if test "$debug_mode" = "no"
then
  CXXFLAGS="$CXXFLAGS -DNDEBUG"
fi

# Checks for programs.
AC_PROG_CXX
AC_PROG_INSTALL

# Checks for libraries.
AC_SEARCH_LIBS([_ZN5boost15program_options18option_descriptionC1Ev], [boost_program_options], , AC_MSG_ERROR([Unable to find Boost Program Options library]))
AC_SEARCH_LIBS([_ZN5boost11basic_regexIiNS_16icu_regex_traitsEE9do_assignEPKiS4_j], [boost_regex], , AC_MSG_ERROR([Unable to find Boost Regex]))


# workaround while http://stackoverflow.com/questions/12679072/ac-search-libs-when-the-function-name-varies 
AC_MSG_CHECKING([whether libicuuc is required])
AC_LINK_IFELSE(	
		[AC_LANG_CALL([[
#include <boost/regex.hpp>
#include <boost/regex/icu.hpp>
boost::u32regex some_regex( boost::make_u32regex( "abc" ) ); 
extern "C"
{
	char test_function();
}
char test_function() { return boost::u32regex_match( "abc", some_regex ) ? '1' : '0'; }
                       ]], [[ test_function ]])], [AC_MSG_RESULT([no])], [AC_MSG_RESULT([yes])] 
[LIBS="$LIBS -licuuc"]
)

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h string.h unistd.h])

# Checks for boost headers
AC_CHECK_HEADERS([boost/program_options.hpp], ,[AC_MSG_ERROR([Unable to find Boost Program Options header (boost/program_options.hpp)])])
AC_CHECK_HEADERS([boost/program_options/options_description.hpp], ,[AC_MSG_ERROR([Unable to find Boost Program Options header (boost/program_options.hpp)])])
AC_CHECK_HEADERS([boost/program_options/cmdline.hpp], ,[AC_MSG_ERROR([Unable to find Boost Program Options header (boost/program_options.hpp)])])
AC_CHECK_HEADERS([boost/program_options/variables_map.hpp], ,[AC_MSG_ERROR([Unable to find Boost Program Options header (boost/program_options.hpp)])])
AC_CHECK_HEADERS([boost/program_options/parsers.hpp], ,[AC_MSG_ERROR([Unable to find Boost Program Options header (boost/program_options.hpp)])])
AC_CHECK_HEADERS([boost/regex.hpp], ,[AC_MSG_ERROR([Unable to find Boost Regex header (boost/regex.hpp)])])
AC_CHECK_HEADERS([boost/regex/icu.hpp], ,[AC_MSG_ERROR([Unable to find Boost Regex header (boost/regex/icu.hpp)])])
AC_CHECK_HEADERS([boost/spirit/include/qi.hpp], , [AC_MSG_ERROR([Unable to find Boost Spirit header (boost/spirit/include/qi.hpp)])])
AC_CHECK_HEADERS([boost/config.hpp], , )

# Checks for memory limitation headers
if test "x${limit_mem}" = xyes; then
  AC_CHECK_HEADERS([sys/resource.h], ,[AC_MSG_ERROR([Unable to find <sys/resource.h>, necessary to build debug functions.])])
fi

# Checks for mmap
AC_CHECK_HEADERS([sys/mman.h], ,[AC_MSG_WARN([mman.h was not found on your computer. Parsing will be slower.])])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
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
