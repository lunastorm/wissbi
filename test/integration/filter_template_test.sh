#!/bin/bash

testNoSink()
{
    cat > $TMP_META_DIR/filterd <<EOF
#!/bin/sh

WISSBI_FILTER_COUNT="1"
WISSBI_FILTER_CMD="sed --unbuffered -e 's/foo/bar/'"
WISSBI_FILTER_SOURCE="foo.in"
WISSBI_FILTER_SINK=""
WISSBI_FILTER_LOG_PREFIX="$TMP_META_DIR/filterd"
WISSBI_FILTER_PID_PREFIX="$TMP_META_DIR/filterd"

WISSBI_META_DIR="$TMP_META_DIR"
WISSBI_SUB_BINARY="$BUILD_DIR/wissbi-sub"
WISSBI_PUB_BINARY="$BUILD_DIR/wissbi-pub"

. $PROJECT_ROOT/scripts/wissbi_filter_template.sh

EOF
    chmod 755 $TMP_META_DIR/filterd
    env WISSBI_META_DIR=$TMP_META_DIR $TMP_META_DIR/filterd start
    sleep 2

    echo "hello foo" | env WISSBI_META_DIR=$TMP_META_DIR $BUILD_DIR/wissbi-pub foo.in
    sleep 1
    $TMP_META_DIR/filterd stop

    assertEquals "hello bar" "`cat $TMP_META_DIR/filterd-1-filter.out`"
}

testFilter()
{
    cat > $TMP_META_DIR/filterd <<EOF
#!/bin/sh

WISSBI_FILTER_COUNT="1"
WISSBI_FILTER_CMD="sed --unbuffered -e 's/foo/bar/'"
WISSBI_FILTER_SOURCE="foo.in"
WISSBI_FILTER_SINK="foo.out"
WISSBI_FILTER_LOG_PREFIX="$TMP_META_DIR/filterd"
WISSBI_FILTER_PID_PREFIX="$TMP_META_DIR/filterd"

WISSBI_META_DIR="$TMP_META_DIR"
WISSBI_SUB_BINARY="$BUILD_DIR/wissbi-sub"
WISSBI_PUB_BINARY="$BUILD_DIR/wissbi-pub"

. $PROJECT_ROOT/scripts/wissbi_filter_template.sh

EOF
    chmod 755 $TMP_META_DIR/filterd
    env WISSBI_META_DIR=$TMP_META_DIR $BUILD_DIR/wissbi-sub foo.out > $TMP_META_DIR/received &
    SUB_PID=$!
    env WISSBI_META_DIR=$TMP_META_DIR $TMP_META_DIR/filterd start
    sleep 2

    echo "hello foo" | env WISSBI_META_DIR=$TMP_META_DIR $BUILD_DIR/wissbi-pub foo.in
    sleep 1
    $TMP_META_DIR/filterd stop
    kill $SUB_PID

    assertEquals "hello bar" "`cat $TMP_META_DIR/received`"
}

testNoSub()
{
    cat > $TMP_META_DIR/filterd <<EOF
#!/bin/sh

WISSBI_FILTER_COUNT="1"
WISSBI_FILTER_CMD="echo 'hello bar'"
WISSBI_FILTER_SOURCE=""
WISSBI_FILTER_SINK="foo.out"
WISSBI_FILTER_LOG_PREFIX="$TMP_META_DIR/filterd"
WISSBI_FILTER_PID_PREFIX="$TMP_META_DIR/filterd"

WISSBI_META_DIR="$TMP_META_DIR"
WISSBI_SUB_BINARY="$BUILD_DIR/wissbi-sub"
WISSBI_PUB_BINARY="$BUILD_DIR/wissbi-pub"

. $PROJECT_ROOT/scripts/wissbi_filter_template.sh

EOF
    chmod 755 $TMP_META_DIR/filterd
    env WISSBI_META_DIR=$TMP_META_DIR $BUILD_DIR/wissbi-sub foo.out > $TMP_META_DIR/received &
    SUB_PID=$!
    env WISSBI_META_DIR=$TMP_META_DIR $TMP_META_DIR/filterd start
    sleep 2

    $TMP_META_DIR/filterd stop
    kill $SUB_PID

    assertEquals "hello bar" "`cat $TMP_META_DIR/received`"
}

testMultipleCount()
{
    cat > $TMP_META_DIR/filterd <<EOF
#!/bin/sh

WISSBI_FILTER_COUNT="4"
WISSBI_FILTER_CMD="cat"
WISSBI_FILTER_SOURCE="foo.in"
WISSBI_FILTER_SINK=""
WISSBI_FILTER_LOG_PREFIX="$TMP_META_DIR/filterd"
WISSBI_FILTER_PID_PREFIX="$TMP_META_DIR/filterd"

WISSBI_META_DIR="$TMP_META_DIR"
WISSBI_SUB_BINARY="$BUILD_DIR/wissbi-sub"
WISSBI_PUB_BINARY="$BUILD_DIR/wissbi-pub"

. $PROJECT_ROOT/scripts/wissbi_filter_template.sh

EOF
    chmod 755 $TMP_META_DIR/filterd
    env WISSBI_META_DIR=$TMP_META_DIR $TMP_META_DIR/filterd start
    sleep 2

    seq -w 1 1000 > $TMP_META_DIR/input
    cat $TMP_META_DIR/input | env WISSBI_META_DIR=$TMP_META_DIR $BUILD_DIR/wissbi-pub foo.in
    sleep 2
    $TMP_META_DIR/filterd stop

    assertNotEquals "0" "`wc -l $TMP_META_DIR/filterd-1-filter.out | cut -d ' ' -f 1`"
    assertNotEquals "0" "`wc -l $TMP_META_DIR/filterd-2-filter.out | cut -d ' ' -f 1`"
    assertNotEquals "0" "`wc -l $TMP_META_DIR/filterd-3-filter.out | cut -d ' ' -f 1`"
    assertNotEquals "0" "`wc -l $TMP_META_DIR/filterd-4-filter.out | cut -d ' ' -f 1`"
    cat $TMP_META_DIR/filterd-*-filter.out | sort > $TMP_META_DIR/output
    assertTrue "received messages are not correct" "diff $TMP_META_DIR/input $TMP_META_DIR/output > /dev/null"
}

oneTimeSetUp()
{
    : ${PROJECT_ROOT:="`(cd ../../ ; pwd)`"}
    : ${BUILD_DIR:="`(cd ../../tmp/build ; pwd)`"}
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
