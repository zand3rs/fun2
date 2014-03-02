#!/bin/bash
./configure
if [ $? != 0 ]; then
    exit 1
fi
. dist-install
