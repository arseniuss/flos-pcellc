#!/bin/sh

# TODO: PASS FAIL etc functions

set -e

PASS() {
    local name="$1"
    local args="$2"
    local data="$3"

    set +e

    ERR=$(echo $data | ./bin/pcellscan -l 2>&1 > /dev/null)

    if [ -e "$ERR" ]; then
        echo error: $ERR
        exit 1
    else
        printf "\t%-30s passed\n" "$name"
    fi
}

FAIL() {
    local name="$1"
    local args="$2"
    local data="$3"
    local exp="$4"

    ERR=$(echo "$data" | ./bin/pcellscan $args 2>&1 > /dev/null)

    if echo $ERR | grep "$exp" > /dev/null; then
        printf "\t%-30s passed\n" "$name"
    else
        echo "expected \"$exp\", got \"$ERR\""
        exit 1
    fi
}

EXPECT() {
    local name="$1"
    local args="$2"
    local data="$3"
    local result="$4"

    echo "$data" | ./bin/pcellscan $args


}

echo "pcellscan coverage test"

#PASS "test 'as' keyword" "" "as"
PASS "test comment" "-l" "/*  */"
EXPECT "checking comment" "-e" "/* */" ""

#FAIL "test usage" "-X" "" "usage: ./bin/pcellscan"


