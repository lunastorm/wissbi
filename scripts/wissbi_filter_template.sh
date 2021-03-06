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

FIFO_SOURCES=`set | grep -E "WISSBI_FILTER_SOURCE[0-9]+" || true`
FIFO_SINKS=`set | grep -E "WISSBI_FILTER_SINK[0-9]+" || true`

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
        WISSBI_RESOLVED_FIFO_SOURCES=`echo "$FIFO_SOURCES" | sed -e "s/\\$i/$i/g"`
        WISSBI_RESOLVED_FIFO_SINKS=`echo "$FIFO_SINKS" | sed -e "s/\\$i/$i/g"`
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

        extra_src_cnt=0
        for FIFO_SOURCE in $FIFO_SOURCES
        do
            FIFO_SOURCE=`echo $FIFO_SOURCE | sed -e "s/.*'\(.*\)'/\1/"`
            EXTRA_FIFO=`echo $FIFO_SOURCE | cut -d ':' -f 1`
            EXTRA_SOURCE=`echo $FIFO_SOURCE | cut -d ':' -f 2`
            mkfifo $EXTRA_FIFO
            chown $WISSBI_RUN_AS $EXTRA_FIFO
            $SH_CMD -c "env WISSBI_META_DIR=$WISSBI_META_DIR $WISSBI_SUB_BINARY $EXTRA_SOURCE > $EXTRA_FIFO 2>/dev/null ; rm -rf $EXTRA_FIFO" $WISSBI_RUN_AS >/dev/null 2>&1 &
            echo $! > $WISSBI_FILTER_PID_PREFIX-extra-$extra_src_cnt.pid
            extra_src_cnt=$((extra_src_cnt+1))
        done

        for FIFO_SINK in $FIFO_SINKS
        do
            FIFO_SINK=`echo $FIFO_SINK | sed -e "s/.*'\(.*\)'/\1/"`
            EXTRA_FIFO=`echo $FIFO_SINK | cut -d ':' -f 1`
            EXTRA_SINK=`echo $FIFO_SINK | cut -d ':' -f 2`
            mkfifo $EXTRA_FIFO
            chown $WISSBI_RUN_AS $EXTRA_FIFO
            $SH_CMD -c "cat $EXTRA_FIFO | env WISSBI_META_DIR=$WISSBI_META_DIR $WISSBI_PUB_BINARY $EXTRA_SINK 2>/dev/null ; rm -rf $EXTRA_FIFO" $WISSBI_RUN_AS >/dev/null 2>&1 &
        done

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

get_child_recursive(){
    pid=$1
    child=`ps --ppid $pid | sed -e '1d; s/^[ ]*//' | cut -d ' ' -f 1`
    if [ -z $child ]
    then
        echo $pid
    else
        echo `get_child_recursive $child`
    fi
}

stop() {
    for pidfile in `ls $WISSBI_FILTER_PID_PREFIX-*.pid 2>/dev/null`
    do
        pid=`cat $pidfile`
        target_pid=`get_child_recursive $pid`
        echo "killing $target_pid"
        kill $target_pid || true
        rm -f $pidfile
    done

    for pidfile in `ls $WISSBI_FILTER_PID_PREFIX-*.pid.wait 2>/dev/null`
    do
        pid=`cat $pidfile`
        while [ -e /proc/$pid ]
        do
            echo "waiting for $pid to die"
            sleep 1
        done
        rm -f $pidfile
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
        pid=`cat $pidfile`
        if [ -n "$pid" ] && [ -e /proc/$pid ]
        then
            echo "$pidfile ($pid) is running"
        else
            echo "$pidfile ($pid) is not running"
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

