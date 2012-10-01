#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([tatoparser], 2.2, [victor.lavaud@gmail.com])
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_INSTALL

# Checks for libraries.
AC_SEARCH_LIBS([_ZN5boost15program_options18option_descriptionC1Ev], [boost_program_options], , AC_MSG_ERROR([Unable to find Boost Program Options library]))
AC_SEARCH_LIBS([_ZN5boost11basic_regexIiNS_16icu_regex_traitsEE9do_assignEPKiS4_j], [boost_regex], , AC_MSG_ERROR([Unable to find Boost Regex]))
#AC_SEARCH_LIBS([u_tolower_49],[icuuc], , AC_MSG_ERROR([Unable to find icuuc, make sure ICU is installed.]))

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT32_T
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T

# Checks for library functions.
AC_FUNC_MMAP
AC_CHECK_FUNCS([munmap])

AC_CONFIG_FILES([Makefile
                 src/Makefile])
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

--------------------------------------------------"
