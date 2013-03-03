#!/bin/sh

set -o nounset
set -o errexit

: ${WISSBI_META_DIR:="/var/lib/wissbi"}
: ${WISSBI_BACKUP_DIR:="/var/lib/wissbi-backup"}

if [ -z "`ls $WISSBI_META_DIR`" ]
then
    logger -s -i -t wsbmetad "restoring wissbi metadata from backup"
    rsync -rdlpAog --delete $WISSBI_BACKUP_DIR/* $WISSBI_META_DIR
fi

while true
do
    rsync -rdlpAog --delete $WISSBI_META_DIR/* $WISSBI_BACKUP_DIR
    for ENTRY in `find $WISSBI_META_DIR/sub | grep ":"`
    do
        ATIME=`stat -c "%X" $ENTRY || true`
        if [ "$((`date +%s` - ATIME))" -gt 60 ]
        then
            rm $ENTRY
            logger -s -i -t wsbmetad "Removed dead entry: $ENTRY"
        fi
    done
    sleep 1
done

