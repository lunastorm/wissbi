#!/bin/bash

testSendOneLineMsg()
{
    env WISSBI_META_DIR=$TMP_META_DIR $BUILD_DIR/wissbi-sub foo > $TMP_META_DIR/received &
    SUB_PID=$!

    sleep 1
    echo "hello world" | env WISSBI_META_DIR=$TMP_META_DIR $BUILD_DIR/wissbi-pub foo
    sleep 1
    kill $SUB_PID

    assertEquals "hello world" "`cat $TMP_META_DIR/received`"
}

testSendRandomMsg()
{
    env WISSBI_META_DIR=$TMP_META_DIR $BUILD_DIR/wissbi-sub foo > $TMP_META_DIR/received &
    SUB_PID=$!

    sleep 1
    dd if=/dev/urandom bs=1M count=1 | base64 > $TMP_META_DIR/input
    cat $TMP_META_DIR/input | env WISSBI_DROP_MESSAGE=no WISSBI_META_DIR=$TMP_META_DIR $BUILD_DIR/wissbi-pub foo
    sleep 1
    kill $SUB_PID

    assertTrue "diff $TMP_META_DIR/input $TMP_META_DIR/received"
}

testRunPubFirst()
{
    echo "hello world" | env WISSBI_META_DIR=$TMP_META_DIR WISSBI_PUB_WAIT_TIMEOUT_SEC=3 $BUILD_DIR/wissbi-pub foo &
    PUB_PID=$!

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
