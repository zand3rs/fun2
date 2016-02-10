dnl
dnl Configure script for fun2...
dnl
dnl $Id$
dnl

AC_INIT([fun2], [1.6.2], [zand3rs@gmail.com])
AM_INIT_AUTOMAKE
AC_CONFIG_HEADER([config.h])

RELEASE_DATE="02/2016"
COPYRIGHT="2009-2016"

PACKAGE_MAJOR_VERSION="${PACKAGE_VERSION%%.*}"
PACKAGE_MINOR_VERSION="${PACKAGE_VERSION#*.}"

LIBCURL="`curl-config --libs`"

AH_TEMPLATE([RELEASE_DATE], [Release date])
AH_TEMPLATE([COPYRIGHT], [Copyright])
AH_TEMPLATE([__DEBUG__], [Debug flag])
AH_TEMPLATE([__DEVEL__], [Development flag])

AC_DEFINE_UNQUOTED([RELEASE_DATE], "$RELEASE_DATE")
AC_DEFINE_UNQUOTED([COPYRIGHT], "$COPYRIGHT")

#AM_PATH_LIBSQLORA8(2.2.0, , [
#        LIBS="$LIBS $LIBSQLORA8_LIBS"
#        CFLAGS="$CPPFLAGS $LIBSQLORA8_CFLAGS"
#        ], [echo "*** Please install libsqlora 2.2.0 or higher"; exit 1])

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

AC_ARG_ENABLE([devel], 
              AC_HELP_STRING([--enable-devel], 
              [Enable/disable devel flag (default is disabled)]), 
              [AC_DEFINE([__DEVEL__],[],[])])

dnl Checks for header files.
AC_HEADER_STDC

dnl Checks for programs.
AC_PROG_CXX
AC_PROG_CPP
AC_PROG_CC
AC_PROG_INSTALL

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
PACKAGE_MAJOR_VERSION
PACKAGE_MINOR_VERSION
LIBCURL
])

AC_OUTPUT([
Makefile 
src/Makefile
dist-install 
])

