#!/bin/bash

testDeadEntryCleanup()
{
    mkdir -p $TMP_META_DIR/sub/foo
    touch -d @$((`date +%s`-120)) $TMP_META_DIR/sub/foo/1.1.1.1:12345,dead.entry
    touch $TMP_META_DIR/sub/foo/1.1.1.1:1234,live.entry

    env WISSBI_META_DIR=$TMP_META_DIR WISSBI_BACKUP_DIR=$TMP_BACKUP_DIR $PROJECT_ROOT/scripts/wsbmetad.sh &
    WSBMETAD_PID=$!
    sleep 1
    kill $WSBMETAD_PID

    assertFalse "dead entry not removed" "ls $TMP_META_DIR/sub/foo/1.1.1.1:12345,dead.entry"
    assertTrue "live entry wrongly removed" "ls $TMP_META_DIR/sub/foo/1.1.1.1:1234,live.entry"
}

testRestoreMeta()
{
    mkdir -p $TMP_META_DIR/sub/foo
    mkdir -p $TMP_META_DIR/sub/foo/bar
    ln -s $TMP_META_DIR/sub/foo/bar $TMP_META_DIR/sub/abc
    env WISSBI_META_DIR=$TMP_META_DIR WISSBI_BACKUP_DIR=$TMP_BACKUP_DIR $PROJECT_ROOT/scripts/wsbmetad.sh &
    WSBMETAD_PID=$!
    sleep 1
    kill $WSBMETAD_PID
    rm -rf $TMP_META_DIR/*

    env WISSBI_META_DIR=$TMP_META_DIR WISSBI_BACKUP_DIR=$TMP_BACKUP_DIR $PROJECT_ROOT/scripts/wsbmetad.sh &
    WSBMETAD_PID=$!
    sleep 1
    kill $WSBMETAD_PID
    assertTrue "metadata not restored" "ls $TMP_META_DIR/sub/foo"
    assertTrue "metadata not restored" "ls $TMP_META_DIR/sub/foo/bar"
    assertTrue "metadata not restored" "ls $TMP_META_DIR/sub/abc"
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
    TMP_BACKUP_DIR=`mktemp -d`
    mkdir -p $TMP_META_DIR/sub
}

tearDown()
{
    rm -rf $TMP_META_DIR
    rm -rf $TMP_BACKUP_DIR
}

. ../../3rd_party/shunit2/src/shell/shunit2
