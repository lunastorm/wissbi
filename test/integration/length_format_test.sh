#!/bin/bash

testSendLengthMsg()
{
    env WISSBI_MESSAGE_FORMAT="length" WISSBI_META_DIR=$TMP_META_DIR $BUILD_DIR/wissbi-sub foo > $TMP_META_DIR/received &
    SUB_PID=$!

    sleep 1
    echo -e "11 hello\nworld" | env WISSBI_MESSAGE_FORMAT="length" WISSBI_META_DIR=$TMP_META_DIR $BUILD_DIR/wissbi-pub foo
    sleep 1
    kill $SUB_PID

    assertEquals "11 hello" "`head -n 1 $TMP_META_DIR/received`"
    assertEquals "world" "`tail -n 1 $TMP_META_DIR/received`"
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
