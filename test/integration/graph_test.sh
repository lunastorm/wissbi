#!/bin/bash

testSendOneLineMsg()
{
    mkdir -p $TMP_META_DIR/sub/foo.out
    ln -s $TMP_META_DIR/sub/foo.out $TMP_META_DIR/sub/bar.in

    env WISSBI_META_DIR=$TMP_META_DIR $BUILD_DIR/wissbi-sub bar.in > $TMP_META_DIR/received &
    SUB_PID=$!

    sleep 1
    echo "hello world" | env WISSBI_META_DIR=$TMP_META_DIR $BUILD_DIR/wissbi-pub foo.out
    sleep 1
    kill $SUB_PID

    assertEquals "hello world" "`cat $TMP_META_DIR/received`"
}

testDuplicateToMultipleDest()
{
    mkdir -p $TMP_META_DIR/sub/foo.out
    ln -s $TMP_META_DIR/sub/foo.out $TMP_META_DIR/sub/bar.in
    ln -s $TMP_META_DIR/sub/foo.out $TMP_META_DIR/sub/bar2.in

    env WISSBI_META_DIR=$TMP_META_DIR $BUILD_DIR/wissbi-sub bar.in > $TMP_META_DIR/received &
    SUB_PID=$!
    env WISSBI_META_DIR=$TMP_META_DIR $BUILD_DIR/wissbi-sub bar2.in > $TMP_META_DIR/received2 &
    SUB2_PID=$!

    sleep 1
    echo "hello world" | env WISSBI_META_DIR=$TMP_META_DIR $BUILD_DIR/wissbi-pub foo.out
    sleep 1
    kill $SUB_PID
    kill $SUB2_PID

    assertEquals "hello world" "`cat $TMP_META_DIR/received`"
    assertEquals "hello world" "`cat $TMP_META_DIR/received2`"
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
