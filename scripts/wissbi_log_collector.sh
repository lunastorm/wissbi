#!/bin/sh

set -o nounset
set -o errexit

FILE_PATTERN=`python -c "from ConfigParser import ConfigParser;conf=ConfigParser();conf.read('/etc/wissbi.conf');print conf.get('logcollectord','file_pattern')"`

if [ -e "./wissbi_log_transformer.py" ]
then
    TRANSFORMER_SCRIPT="./wissbi_log_transformer.py"
else
    TRANSFORMER_SCRIPT="/usr/bin/wissbi_log_transformer.py"
fi

FIFO_DIR=`mktemp -d`
FIFO=$FIFO_DIR/fifo
mkfifo $FIFO

tail -n 0 -F $FILE_PATTERN > $FIFO 2>/dev/null &
SRC_PID=$!

cat $FIFO | $TRANSFORMER_SCRIPT &

trap "kill $SRC_PID ; rm -rf $FIFO_DIR" EXIT INT TERM

wait
