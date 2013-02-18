#!/bin/bash

testDeadEntryCleanup()
{
    mkdir -p $TMP_META_DIR/sub/foo
    touch -d @$((`date +%s`-120)) $TMP_META_DIR/sub/foo/dead_entry
    touch $TMP_META_DIR/sub/foo/live_entry

    env WISSBI_META_DIR=$TMP_META_DIR $PROJECT_ROOT/scripts/wsbmetad.sh &
    WSBMETAD_PID=$!
    sleep 1
    kill $WSBMETAD_PID

    assertFalse "dead entry not removed" "ls $TMP_META_DIR/sub/foo/dead_entry"
    assertTrue "live entry wrongly removed" "ls $TMP_META_DIR/sub/foo/live_entry"
}

oneTimeSetUp()
{
    : ${PROJECT_ROOT:="`(cd ../../ ; pwd)`"}
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
