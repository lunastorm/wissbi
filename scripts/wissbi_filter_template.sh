#!/bin/sh

set -o nounset
set -o errexit

: ${WISSBI_RUN_AS:=""}
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
    WISSBI_RECORD_CMD="$WISSBI_RECORD_BINARY"
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
    if [ "$WISSBI_RUN_AS" = "`whoami`" ]
    then
        SH_CMD="/bin/sh"
    else
        SH_CMD="/bin/su"
    fi
    for i in `seq 1 $WISSBI_FILTER_COUNT`
    do
        WISSBI_RESOLVED_FILTER_CMD=`echo "$WISSBI_FILTER_CMD" | sed -e "s/\\$i/$i/g"`
        if [ -n "$WISSBI_DEBUG_DUMP" ]
        then
            WISSBI_DUMP_NAME=$WISSBI_DEBUG_DUMP-$i.dump
        else
            WISSBI_DUMP_NAME=""
        fi
        FIFO_DIR=`mktemp -d`
        FIFO=$FIFO_DIR/fifo
        mkfifo $FIFO
        chown -R $WISSBI_RUN_AS $FIFO_DIR

        if [ -z "$WISSBI_FILTER_SOURCE" ]
        then
            $SH_CMD -c "exec $WISSBI_RESOLVED_FILTER_CMD >$FIFO 2>$WISSBI_FILTER_LOG_PREFIX-$i-filter.err" $WISSBI_RUN_AS >/dev/null 2>&1 &
            echo $! > $WISSBI_FILTER_PID_PREFIX-$i.pid
            $SH_CMD -c "cat $FIFO | env WISSBI_META_DIR=$WISSBI_META_DIR $WISSBI_PUB_BINARY $WISSBI_FILTER_SINK 2>$WISSBI_FILTER_LOG_PREFIX-$i-pub.err ; rm -rf $FIFO_DIR" $WISSBI_RUN_AS >/dev/null 2>&1 &
            echo $! > $WISSBI_FILTER_PID_PREFIX-$i.pid.wait
        elif [ -z $WISSBI_FILTER_SINK ]
        then
            $SH_CMD -c "exec env WISSBI_META_DIR=$WISSBI_META_DIR $WISSBI_SUB_BINARY $WISSBI_FILTER_SOURCE >$FIFO 2>$WISSBI_FILTER_LOG_PREFIX-$i-sub.err" $WISSBI_RUN_AS >/dev/null 2>&1 &
            echo $! > $WISSBI_FILTER_PID_PREFIX-$i.pid
            $SH_CMD -c "cat $FIFO | `if [ -n "$WISSBI_RECORD_CMD" ]; then echo \"$WISSBI_RECORD_CMD $WISSBI_DUMP_NAME 2>$WISSBI_FILTER_LOG_PREFIX-$i-dump.err | \"; fi` $WISSBI_RESOLVED_FILTER_CMD >$WISSBI_FILTER_LOG_PREFIX-$i-filter.out 2>>$WISSBI_FILTER_LOG_PREFIX-$i-filter.err ; rm -rf $FIFO_DIR" $WISSBI_RUN_AS >/dev/null 2>&1 &
            echo $! > $WISSBI_FILTER_PID_PREFIX-$i.pid.wait
        else
            $SH_CMD -c "exec env WISSBI_META_DIR=$WISSBI_META_DIR $WISSBI_SUB_BINARY $WISSBI_FILTER_SOURCE >$FIFO 2>$WISSBI_FILTER_LOG_PREFIX-$i-sub.err" $WISSBI_RUN_AS >/dev/null 2>&1 &
            echo $! > $WISSBI_FILTER_PID_PREFIX-$i.pid
            $SH_CMD -c "cat $FIFO | `if [ -n "$WISSBI_RECORD_CMD" ]; then echo \"$WISSBI_RECORD_CMD $WISSBI_DUMP_NAME 2>$WISSBI_FILTER_LOG_PREFIX-$i-dump.err | \"; fi` $WISSBI_RESOLVED_FILTER_CMD 2>$WISSBI_FILTER_LOG_PREFIX-$i-filter.err | env WISSBI_META_DIR=$WISSBI_META_DIR $WISSBI_PUB_BINARY $WISSBI_FILTER_SINK 2>$WISSBI_FILTER_LOG_PREFIX-$i-pub.err ; rm -rf $FIFO_DIR" $WISSBI_RUN_AS >/dev/null 2>&1 &
            echo $! > $WISSBI_FILTER_PID_PREFIX-$i.pid.wait
        fi
    done

    if status
    then
        echo $0 is started
    else
        echo $0 is not started
    fi
}

stop() {
    for pidfile in `ls $WISSBI_FILTER_PID_PREFIX-*.pid 2>/dev/null`
    do
        pid=`cat $pidfile`
        echo "killing $pid"
        kill $pid || true
        rm -f $pidfile
    done

    for pidfile in `ls $WISSBI_FILTER_PID_PREFIX-*.pid.wait 2>/dev/null`
    do
        pid=`cat $pidfile`
        while [ -e /proc/$pid ]
        do
            echo "waiting for `cat /proc/$pid/cmdline || true` to die"
            sleep 1
        done
    done
    echo $0 is stopped
}

status() {
    if ! ls $WISSBI_FILTER_PID_PREFIX-*.pid >/dev/null 2>&1
    then
        echo "$0 is not running"
        exit 1
    fi
    retval=0
    for pidfile in `ls $WISSBI_FILTER_PID_PREFIX-*.pid 2>/dev/null`
    do
        if kill -0 `cat $pidfile` 2>/dev/null
        then
            echo "$pidfile (`cat $pidfile`) is running"
        else
            echo "$pidfile (`cat $pidfile`) is not running"
            retval=1
        fi
    done
    return $retval
}

check_previlege() {
    if [ -n "$WISSBI_RUN_AS" ]
    then
        if [ "`whoami`" != "root" ]
        then
            echo "Requires root previlege to run as $WISSBI_RUN_AS"
            exit 1
        fi
    else
        WISSBI_RUN_AS=`whoami`
    fi
}

set +o nounset
case "$1" in
start)
    check_previlege
    stop
    start
    ;;
stop)
    check_previlege
    stop
    ;;
restart)
    check_previlege
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

