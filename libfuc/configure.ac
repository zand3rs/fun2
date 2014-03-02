dnl
dnl Configure script for libfuc...
dnl
dnl $Id$
dnl

AC_INIT([libfuc], [0.3.9], [zand3rs@gmail.com])

RELEASE_DATE="07/2009"
COPYRIGHT="2009"

MAINDIR="`cd ${0%/*}; pwd`"
SUBDIRS="src"
CONFIGHEADER="include/config.h"

PACKAGE_MAJOR_VERSION="${PACKAGE_VERSION%%.*}"
PACKAGE_MINOR_VERSION="${PACKAGE_VERSION#*.}"

AC_CONFIG_HEADER([$CONFIGHEADER])

AC_PROG_CC
AC_LANG_C

AC_PROG_MAKE_SET

AC_DEFINE_UNQUOTED([C_VERSION], "$PACKAGE_VERSION")
AC_DEFINE_UNQUOTED([C_RELEASE_DATE], "$RELEASE_DATE")
AC_DEFINE_UNQUOTED([C_COPYRIGHT], "$COPYRIGHT")

AC_ARG_ENABLE([64bit], 
              AC_HELP_STRING([--enable-64bit], 
              [Enable/disable 64bit compilation flags (default is disabled)]), 
              [X64_CFLAGS=-m64])

AC_ARG_ENABLE([debug], 
              AC_HELP_STRING([--enable-debug], 
              [Enable/disable debugging info (default is disabled)]), 
              [AC_DEFINE([__DEBUG__],[],[])])

AC_ARG_WITH([mysql], 
            AC_HELP_STRING([--with-mysql], 
            [path to mysql]), 
            [ac_mysql_prefix=$withval], [])
AC_ARG_WITH([mysql-include], 
            AC_HELP_STRING([--with-mysql-include], 
            [path to mysql include]), 
            [ac_mysql_include=$withval], [])
AC_ARG_WITH([mysql-lib], 
            AC_HELP_STRING([--with-mysql-lib], 
            [path to mysql lib]), 
            [ac_mysql_lib=$withval], [])

AC_ARG_ENABLE([mysql], 
              AC_HELP_STRING([--enable-mysql], 
              [Enable/disable mysql (default is disabled)]), 
              [AC_DEFINE([__MYSQL__],[],[])
                dnl find mysql headers and libraries..
                if test "x$ac_mysql_prefix" != "x" ; then
                    ac_mysql_prefix=${ac_mysql_prefix%/}
                    if test "${ac_mysql_prefix##*/}" = "mysql" ; then
                        ac_mysql_prefix=${ac_mysql_prefix%/*}
                    fi
                else
                    ac_mysql_prefix="/usr /usr/local"
                fi

                echo -n "checking for mysql includes... "
                if test "x$ac_mysql_include" != "x" ; then
                    MYSQL_INCPATH="-I$ac_mysql_include"
                else
                    for tryprefix in $ac_mysql_prefix; do
                        if test -d "$tryprefix/mysql" ; then
                            MYSQL_INCPATH="-I$tryprefix/mysql/include"
                        elif test -d "$tryprefix/include/mysql" ; then
                            MYSQL_INCPATH="-I$tryprefix/include/mysql"
                        fi
                    done
                fi

                if test "x$MYSQL_INCPATH" != "x"; then
                    echo "$MYSQL_INCPATH"
                else
                    echo "not found"
                    echo "***Cannot find mysql: is mysql-devel installed?***"
                    exit 1
                fi

                echo -n "checking for mysql libs... "
                if test "x$ac_mysql_lib" != "x" ; then
                    MYSQL_LIBPATH="-L$ac_mysql_lib"
                else
                    for tryprefix in $ac_mysql_prefix; do
                        if test -d "$tryprefix/mysql" ; then
                            MYSQL_LIBPATH="-L$tryprefix/mysql/lib"
                        elif test -d "$tryprefix/lib/mysql" ; then
                            MYSQL_LIBPATH="-L$tryprefix/lib/mysql"
                        fi
                    done
                fi

                if test "x$MYSQL_LIBPATH" != "x"; then
                    echo "$MYSQL_LIBPATH"
                else
                    echo "not found"
                    echo "***Cannot find mysql: is mysql-devel installed?***"
                    exit 1
                fi

                MYSQL_LLIB="-lmysqlclient"
                MYDB_OBJ="mydb.o"
              ])

AC_HEADER_STDC

AC_CHECK_LIB(pthread, pthread_create, ,AC_MSG_ERROR([pthread_create not functioning]))
AC_CHECK_LIB(socket, socket)
AC_CHECK_LIB(nsl, main)
AC_CHECK_LIB(rt, clock_gettime)

dnl Use -Wall if we have gcc.
changequote(,)dnl
if test "x$GCC" = "xyes"; then
  case " $CFLAGS " in
  *[\ \	]-Wall[\ \	]*) ;;
  *) CFLAGS="$CFLAGS -Wall" ;;
  esac
fi
changequote([,])dnl

dnl -- check for 64 bit platform
PLATFORM="`uname -i`"
if test "x$PLATFORM" = "xx86_64"; then
  X64_CFLAGS=-m64
fi

AC_SUBST([
MAINDIR
SUBDIRS
CONFIGHEADER
PACKAGE_MAJOR_VERSION
PACKAGE_MINOR_VERSION
X64_CFLAGS
MYSQL_INCPATH
MYSQL_LIBPATH
MYSQL_LLIB
MYDB_OBJ
])

AC_OUTPUT([
Makefile 
src/Makefile
])

