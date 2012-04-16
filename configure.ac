#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
#

AC_PREREQ(2.50)
AC_INIT(tssplitter, 1.0, ptriller@soapwars.de)
AC_CONFIG_SRCDIR([src/dvbparser.cc])
AM_INIT_AUTOMAKE   
AM_CONFIG_HEADER([config.h])
AC_CONFIG_FILES([Makefile
	src/Makefile])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_LANG([C++]) 
#CXXFLAGS="$CXXFLAGS -std=gnu++0x" 
CXXCPPFLAGS="$CXXCPPFLAGS -std=gnu++0x"
CPPFLAGS="$CPPFLAGS -std=gnu++0x"
AC_CHECK_HEADER(cstdint) 
AC_SYS_LARGEFILE


# Checks for libraries.

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_OUTPUT

