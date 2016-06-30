#!/bin/bash

ROOTDIR="`pwd`"
EZXMLDIR="ezxml"
MXMLDIR="mxml"
LIBFUCDIR="libfuc"
FUN2RCDIR="fun2rc"
SUBDIRS="$LIBFUCDIR $EZXMLDIR $MXMLDIR $FUN2RCDIR"

for SUBDIR in $SUBDIRS; do
    cd $SUBDIR
    if [ -x "configure" ]; then
        ./configure
        if [ $? != 0 ]; then
            exit 1
        fi
    fi
    make
    if [ $? != 0 ]; then
        exit 1
    fi
    cd $ROOTDIR
done

if [ ! -f "Makefile" ]; then
    ./configure
    if [ $? != 0 ]; then
        exit 1
    fi
fi
make
if [ $? != 0 ]; then
    exit 1
fi

exit 0
