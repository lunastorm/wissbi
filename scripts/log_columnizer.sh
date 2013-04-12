#!/bin/sh

set -o errexit
set -o nounset

COLS=`tput cols`
HEADER="HOST\tFILE\tLOG\n"
BUFFER="$HEADER"
LINE_CNT=0
TERM_HEIGHT=`tput lines`

while read LINE
do
    BUFFER="$BUFFER$LINE\n"
    LINE_CNT=$((LINE_CNT+1))

    if [ $LINE_CNT -gt $((TERM_HEIGHT / 2)) ]
    then
        echo -n "$BUFFER" | column -t -c 3
        BUFFER="$HEADER"
        LINE_CNT=0
    fi
done

if [ $LINE_CNT -gt 0 ]
then
    echo -n "$BUFFER" | column -t -c 3
fi

