#!/bin/bash

testSendOneLineMsg()
{
    env WISSBI_SUB_TOPIC=true WISSBI_META_DIR=$TMP_META_DIR $BUILD_DIR/wissbi-sub foo > $TMP_META_DIR/received1 &
    SUB1_PID=$!
    env WISSBI_SUB_TOPIC=true WISSBI_META_DIR=$TMP_META_DIR $BUILD_DIR/wissbi-sub foo > $TMP_META_DIR/received2 &
    SUB2_PID=$!

    sleep 1
    echo "hello world" | env WISSBI_META_DIR=$TMP_META_DIR $BUILD_DIR/wissbi-pub foo
    sleep 1
    kill $SUB1_PID
    kill $SUB2_PID

    assertEquals "hello world" "`cat $TMP_META_DIR/received1`"
    assertEquals "hello world" "`cat $TMP_META_DIR/received2`"
}

testSendMultipleMsg()
{
    env WISSBI_SUB_TOPIC=true WISSBI_META_DIR=$TMP_META_DIR $BUILD_DIR/wissbi-sub foo > $TMP_META_DIR/received1 &
    SUB1_PID=$!
    env WISSBI_SUB_TOPIC=true WISSBI_META_DIR=$TMP_META_DIR $BUILD_DIR/wissbi-sub foo > $TMP_META_DIR/received2 &
    SUB2_PID=$!

    sleep 1
    echo "hello world" | env WISSBI_META_DIR=$TMP_META_DIR $BUILD_DIR/wissbi-pub foo
    echo "hello world2" | env WISSBI_META_DIR=$TMP_META_DIR $BUILD_DIR/wissbi-pub foo
    echo "hello world3" | env WISSBI_META_DIR=$TMP_META_DIR $BUILD_DIR/wissbi-pub foo
    sleep 2
    kill $SUB1_PID
    kill $SUB2_PID

    assertEquals "hello world" "`sed -e '1p; d' $TMP_META_DIR/received1`"
    assertEquals "hello world2" "`sed -e '2p; d' $TMP_META_DIR/received1`"
    assertEquals "hello world3" "`sed -e '3p; d' $TMP_META_DIR/received1`"
    assertEquals "hello world" "`sed -e '1p; d' $TMP_META_DIR/received2`"
    assertEquals "hello world2" "`sed -e '2p; d' $TMP_META_DIR/received2`"
    assertEquals "hello world3" "`sed -e '3p; d' $TMP_META_DIR/received2`"
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
