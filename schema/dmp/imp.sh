#!/bin/bash

if [ $# -ne 3 ]; then
    echo "$0 <ora_auth> <file> <touser>"
    exit 1
fi

ORA_AUTH="$1"
FILE="$2"
TOUSER="$3"

imp $ORA_AUTH file=$FILE fromuser=trigger_owner touser=$TOUSER grants=no

