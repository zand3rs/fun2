#!/bin/bash

ROOTDIR="`pwd`"
EZXMLDIR="ezxml"
MXMLDIR="mxml"
LIBFUCDIR="libfuc"
FUN2RCDIR="fun2rc"
SUBDIRS="$LIBFUCDIR $FUN2RCDIR"

if [ ! -f "Makefile" ]; then
    ./configure
    if [ $? != 0 ]; then
        echo "Failed to create distribution package!"
        exit 1
    fi
fi

for SUBDIR in $SUBDIRS; do
    cd $SUBDIR
    if [ -f "Makefile" ]; then
        make distclean
    fi
    cd $ROOTDIR
done

make dist
if [ $? != 0 ]; then
    echo "Failed to create distribution package!"
    exit 1
fi

echo "Distribution package created."

exit 0
