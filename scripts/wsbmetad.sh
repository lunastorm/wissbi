#!/bin/sh

set -o nounset
set -o errexit

: ${WISSBI_META_DIR:="/var/lib/wissbi"}

while true
do
    for ENTRY in `find $WISSBI_META_DIR/sub`
    do
        ATIME=`stat -c "%X" $ENTRY || true`
        if [ "$((`date +%s` - ATIME))" -gt 60 ]
        then
            rm $ENTRY
        fi
    done
    sleep 1
done

