dnl
dnl Configure script for fun2rc...
dnl
dnl $Id$
dnl

AC_INIT([fun2rc], [0.5.0], [zand3rs@gmail.com])

RELEASE_DATE="02/2017"
COPYRIGHT="2009-2017"

MAINDIR="`cd ${0%/*}; pwd`"
SUBDIRS="`cd ${0%/*}; cat subdirs`"
MAKEFILES="`cd ${0%/*}; cat subdirs | sed -e 's/\([[^ ]]*\)/\1\/Makefile/g'`"
CONFIGHEADER="include/config.h"

PACKAGE_MAJOR_VERSION="${PACKAGE_VERSION%%.*}"
PACKAGE_MINOR_VERSION="${PACKAGE_VERSION#*.}"

LIBCURL="`curl-config --libs`"

AC_CONFIG_HEADER([$CONFIGHEADER])

dnl Checks for programs.
AC_PROG_CXX
AC_PROG_CPP
AC_PROG_CC
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_RANLIB

AC_DEFINE_UNQUOTED([C_VERSION], "$PACKAGE_VERSION")
AC_DEFINE_UNQUOTED([C_RELEASE_DATE], "$RELEASE_DATE")
AC_DEFINE_UNQUOTED([C_COPYRIGHT], "$COPYRIGHT")

dnl
dnl Check if the required libsqlora8 is installed
dnl
PKG_CHECK_MODULES(LIBSQLORA8, [ libsqlora8 >= 2.3.0 ], [
        LIBS="$LIBS $LIBSQLORA8_LIBS"
        CFLAGS="$CFLAGS $LIBSQLORA8_CFLAGS"
        ],[ echo "*** Please install libsqlora8 2.3.0 or higher"; exit 1 ])

AC_ARG_ENABLE([debug], 
              AC_HELP_STRING([--enable-debug], 
              [Enable/disable debugging info (default is disabled)]), 
              [AC_DEFINE([__DEBUG__],[],[])])

dnl Checks for header files.
AC_HEADER_STDC

dnl Checks for libraries.
AC_CHECK_LIB(pthread, pthread_create, ,AC_MSG_ERROR([pthread_create not functioning]))
AC_CHECK_LIB(socket, socket)
AC_CHECK_LIB(nsl, main)
AC_CHECK_LIB(rt, main)
AC_CHECK_LIB(dl, main)

dnl Use -Wall if we have gcc.
changequote(,)dnl
if test "x$GCC" = "xyes"; then
  case " $CFLAGS " in
  *[\ \	]-Wall[\ \	]*) ;;
  *) CFLAGS="$CFLAGS -Wall" ;;
  esac
fi
changequote([,])dnl

AC_SUBST([
MAINDIR
SUBDIRS
CONFIGHEADER
PACKAGE_MAJOR_VERSION
PACKAGE_MINOR_VERSION
LIBCURL
])

AC_OUTPUT([
Makefile 
$MAKEFILES
])

