#!/bin/bash

testForceKillSub2()
{
    for PORT in `seq 32768 61000`
    do
        nc -l $PORT &
        FAKE_SUB_PID=$!
        sleep 1
        if kill -0 $FAKE_SUB_PID
        then
            break
        fi
    done
    mkdir -p $TMP_META_DIR/sub/foo
    touch $TMP_META_DIR/sub/foo/127.0.0.1:$PORT,foo
    sleep 1

    mkfifo $TMP_META_DIR/fifo
    cat $TMP_META_DIR/fifo | env WISSBI_META_DIR=$TMP_META_DIR WISSBI_PUB_WAIT_TIMEOUT_SEC=10 $BUILD_DIR/wissbi-pub foo &
    PUB_PID=$!
    sleep 1

    kill $FAKE_SUB_PID

    sleep 1

    echo "hello world" > $TMP_META_DIR/fifo

    env WISSBI_META_DIR=$TMP_META_DIR $BUILD_DIR/wissbi-sub foo > $TMP_META_DIR/received &
    SUB_PID=$!

    sleep 3

    kill $PUB_PID
    kill $SUB_PID

    assertEquals "hello world" "`cat $TMP_META_DIR/received`"
}

oneTimeSetUp()
{
    if [ -z "$BUILD_DIR" ]
    then
        BUILD_DIR=../../tmp/build/src
    fi
}

oneTimeTearDown()
{
    true
}

setUp()
{
    TMP_META_DIR=`mktemp -d`
    mkdir -p $TMP_META_DIR/sub
}

tearDown()
{
    rm -rf $TMP_META_DIR
}

. ../../3rd_party/shunit2/src/shell/shunit2
