#!/bin/bash

testForceKillSub2()
{
    env WISSBI_META_DIR=$TMP_META_DIR $BUILD_DIR/wissbi-sub foo &
    SUB_PID=$!
    sleep 1

    mkfifo $TMP_META_DIR/fifo
    cat $TMP_META_DIR/fifo | env WISSBI_META_DIR=$TMP_META_DIR WISSBI_PUB_WAIT_TIMEOUT_SEC=10 $BUILD_DIR/wissbi-pub foo &
    PUB_PID=$!
    sleep 1

    kill -9 $SUB_PID

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
        BUILD_DIR=../../tmp/build
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