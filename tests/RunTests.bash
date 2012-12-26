#!/bin/bash

TEST_DIR="$(dirname "$0")"
INCLUDE_DIR="$TEST_DIR/../include"
LIBRHO_PATH="$TEST_DIR/../objects/librho.a"
OUT_FILE="a.out"

CC="g++"
CC_FLAGS+="-O0 -fno-inline -g -rdynamic -Wall -Werror -I $INCLUDE_DIR"
CC_LIB_FLAGS+="$LIBRHO_PATH"

if [ $(uname) == "Darwin" ]
then
    CC_LIB_FLAGS+=" -lpthread -lglfw"
    CC_LIB_FLAGS+=" -framework Foundation -framework AVFoundation "
    CC_LIB_FLAGS+=" -framework CoreVideo -framework CoreMedia "
    CC_LIB_FLAGS+=" -framework OpenGL -framework IOKit "
    CC_LIB_FLAGS+=" -framework Cocoa -framework GLUT"
else
    CC_LIB_FLAGS+=" -lpthread -lglut -lglfw -lGLU"
fi

if [ -n "$1" ]
then
    TEST_DIR="$1"
fi

function cleanup
{
    rm -rf "$OUT_FILE"*
}

function gotSignal
{
    echo
    if [ -n $testPid ]
    then
        echo "Killing process $testPid ..."
        kill -SIGTERM $testPid
    fi
    cleanup
    echo "Exiting due to signal ..."
    exit 1
}

trap gotSignal SIGTERM SIGINT

somethingRun=0

for testPath in $(find "$TEST_DIR" -name '*.cpp' -o -name '*.mm')
do
    somethingRun=1
    echo "---- $testPath ----"
    $CC $CC_FLAGS $testPath $CC_LIB_FLAGS -o "$OUT_FILE"
    compileStatus=$?
    if [ $compileStatus -eq 0 ]
    then
        ./"$OUT_FILE" &
        testPid=$!
        wait $testPid
        testStatus=$?
        echo
        if [ $testStatus -ne 0 ]
        then
            cleanup
            echo "^^^^^^^^ TEST FAILURE ^^^^^^^^"
            exit 1
        fi
    else
        cleanup
        echo
        echo "^^^^^^^^ COMPILATION ERROR ^^^^^^^^"
        exit 1
    fi
done

if [ $somethingRun -eq 0 ]
then
    echo "^^^^^^^^ NO TESTS WHERE RUN ^^^^^^^^"
    exit 1
fi

cleanup
echo "____ ALL TESTS PASSED ____"
exit 0
