#!/bin/bash

testSendOneLineMsg()
{
    env WISSBI_META_DIR=$TMP_META_DIR $BUILD_DIR/sub foo > $TMP_META_DIR/received &
    SUB_PID=$!

    sleep 1
    echo "hello world" | env WISSBI_META_DIR=$TMP_META_DIR $BUILD_DIR/pub foo 
    sleep 1
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
