#!/bin/bash

TEST_DIR="$(dirname "$0")"
INCLUDE_DIR="$TEST_DIR/../include"
LIBRHO_PATH="$TEST_DIR/../objects/librho.a"
OUT_FILE="a.out"

CC="g++"
CC_FLAGS="-O0 -fno-inline -g -rdynamic -Wall -Werror -I $INCLUDE_DIR"
CC_LIB_FLAGS="$LIBRHO_PATH -lpthread"

CC_FRAMEWORK_FLAGS=
if [ $(uname) == "Darwin" ]
then
    CC_FRAMEWORK_FLAGS+="-framework Foundation -framework AVFoundation "
    CC_FRAMEWORK_FLAGS+=" -framework CoreVideo -framework CoreMedia "
fi

if [ -n "$1" ]
then
    TEST_DIR="$1"
fi

for testPath in $(find "$TEST_DIR" -name '*.cpp')
do
    echo "---- $testPath ----"
    $CC $CC_FLAGS $testPath $CC_LIB_FLAGS $CC_FRAMEWORK_FLAGS -o "$OUT_FILE"
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
