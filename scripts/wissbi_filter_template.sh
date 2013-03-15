#!/bin/sh

set -o nounset
set -o errexit

: ${WISSBI_FILTER_COUNT:="1"}
: ${WISSBI_FILTER_CMD:=""}
: ${WISSBI_FILTER_SOURCE:=""}
: ${WISSBI_FILTER_SINK:=""}
: ${WISSBI_FILTER_LOG_PREFIX:=""}
: ${WISSBI_FILTER_PID_PREFIX:=""}

: ${WISSBI_META_DIR:="/var/lib/wissbi"}
: ${WISSBI_SUB_BINARY:="/usr/bin/wissbi-sub"}
: ${WISSBI_PUB_BINARY:="/usr/bin/wissbi-pub"}
: ${WISSBI_RECORD_BINARY:="/usr/bin/wissbi-record"}

if [ "${WISSBI_DEBUG_DUMP+set}" = set ]
then
    WISSBI_RECORD_CMD="$WISSBI_RECORD_BINARY $WISSBI_DEBUG_DUMP"
else
    WISSBI_RECORD_CMD=""
fi

if [ -z "$WISSBI_FILTER_CMD" ]
then
    echo "You have to set a non empty WISSBI_FILTER_CMD"
    exit 1
fi

if [ -z "$WISSBI_FILTER_PID_PREFIX" ]
then
    echo "You have to set a non empty WISSBI_FILTER_PID_PREFIX"
    exit 1
fi

if [ -z "$WISSBI_FILTER_LOG_PREFIX" ]
then
    echo "You have to set a non empty WISSBI_FILTER_LOG_PREFIX"
    exit 1
fi

if [ -z "$WISSBI_FILTER_SOURCE" ] && [ -z "$WISSBI_FILTER_SINK" ]
then
    echo "cannot leave blank for both source and sink"
    exit 1
fi


start() {
    for i in `seq 1 $WISSBI_FILTER_COUNT`
    do
        FIFO_DIR=`mktemp -d`
        FIFO=$FIFO_DIR/fifo
        mkfifo $FIFO

        if [ -z "$WISSBI_FILTER_SOURCE" ]
        then
            /bin/sh -c "exec $WISSBI_FILTER_CMD" >$FIFO 2>$WISSBI_FILTER_LOG_PREFIX-$i-filter.err &
            echo $! > $WISSBI_FILTER_PID_PREFIX-$i.pid
            /bin/sh -c "cat $FIFO | env WISSBI_META_DIR=$WISSBI_META_DIR $WISSBI_PUB_BINARY $WISSBI_FILTER_SINK 2>$WISSBI_FILTER_LOG_PREFIX-$i-pub.err ; rm -rf $FIFO_DIR" &>/dev/null &
        elif [ -z $WISSBI_FILTER_SINK ]
        then
            env WISSBI_META_DIR=$WISSBI_META_DIR $WISSBI_SUB_BINARY $WISSBI_FILTER_SOURCE >$FIFO 2>$WISSBI_FILTER_LOG_PREFIX-$i-sub.err &
            echo $! > $WISSBI_FILTER_PID_PREFIX-$i.pid
            /bin/sh -c "cat $FIFO | `if [ -n $WISSBI_RECORD_CMD ]; then echo \"$WISSBI_RECORD_CMD 2>$WISSBI_FILTER_LOG_PREFIX-$i-dump.err | \"; fi` $WISSBI_FILTER_CMD >$WISSBI_FILTER_LOG_PREFIX-$i-filter.out 2>>$WISSBI_FILTER_LOG_PREFIX-$i-filter.err ; rm -rf $FIFO_DIR" &>/dev/null &
        else
            env WISSBI_META_DIR=$WISSBI_META_DIR $WISSBI_SUB_BINARY $WISSBI_FILTER_SOURCE >$FIFO 2>$WISSBI_FILTER_LOG_PREFIX-$i-sub.err &
            echo $! > $WISSBI_FILTER_PID_PREFIX-$i.pid
            /bin/sh -c "cat $FIFO | `if [ -n $WISSBI_RECORD_CMD ]; then echo \"$WISSBI_RECORD_CMD 2>$WISSBI_FILTER_LOG_PREFIX-$i-dump.err | \"; fi` $WISSBI_FILTER_CMD 2>$WISSBI_FILTER_LOG_PREFIX-$i-filter.err | env WISSBI_META_DIR=$WISSBI_META_DIR $WISSBI_PUB_BINARY $WISSBI_FILTER_SINK 2>$WISSBI_FILTER_LOG_PREFIX-$i-pub.err ; rm -rf $FIFO_DIR" &>/dev/null &
        fi
    done
}

stop() {
    for pidfile in `ls $WISSBI_FILTER_PID_PREFIX-*.pid`
    do
        kill `cat $pidfile` || true
        rm -f $pidfile
    done
}

status() {
    retval=0
    for pidfile in `ls $WISSBI_FILTER_PID_PREFIX-*.pid`
    do
        if kill -0 `cat $pidfile` 2>/dev/null
        then
            echo "$pidfile (`cat $pidfile`) is running"
        else
            echo "$pidfile (`cat $pidfile`) is not running"
            retval=1
        fi
    done
    exit $retval
}

set +o nounset
case "$1" in
start)
    start
    ;;
stop)
    stop
    ;;
restart)
    stop
    start
    ;;
status)
    status
    ;;
*)
    echo "Usage: $0 {start|stop|restart|status}"
    ;;
esac

