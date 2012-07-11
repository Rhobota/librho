#!/bin/bash

SEARCH_DIR="."
OUT_FILE="a.out"

CC="g++"
CC_FLAGS="-O0 -fno-inline -g -rdynamic -I ../"
CC_LIB_FLAGS="-lpthread"

if [ -n "$1" ]
then
    SEARCH_DIR="$1"
fi

for testPath in $(find "$SEARCH_DIR" -name '*.cpp')
do
    echo "---- $testPath ----"
    $CC $CC_FLAGS $testPath $CC_LIB_FLAGS -o "$OUT_FILE"
    compileStatus=$?
    if [ $compileStatus -eq 0 ]
    then
        ./"$OUT_FILE"
        testStatus=$?
        echo
        if [ $testStatus -ne 0 ]
        then
            echo "^^^^^^^^ TEST FAILURE ^^^^^^^^"
            exit 1
        fi
    else
        echo
        echo "^^^^^^^^ COMPILATION ERROR ^^^^^^^^"
        exit 1
    fi
done

rm -rf "$OUT_FILE"*

exit 0
