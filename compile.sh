#!/bin/bash

unittest=false
while getopts ":p:t" opt; do
    case $opt in
        p)
            project="$OPTARG"
            project=${project%/}
            ;;
        t)
            unittest=true
        ;;
        \?)
            echo "无效的选项: -$OPTARG" >&2
            ;;
    esac
done

# log
info() {
    echo -- "\e[32m$1\e[0m\n" 
}

error() {
    echo -- "\e[31mERROR: $1\e[0m"
}

FILE_NAME="$0"
PROJECT_NAME="$project"

# download third-party libraries
git submodule update --init

# GTest
UNITTEST='./core/unittest'

# compile
if [ -d "$PROJECT_NAME" ]; then
    cd "$PROJECT_NAME"
    if [ ! -d build ]; then
        mkdir build
    fi
    cd build

    # clean executation
    if [ -x "$PROJECT_NAME" ]; then
        rm "$PROJECT_NAME"
    fi

    if [ -x "./../$PROJECT_NAME" ]; then
        rm "./../$PROJECT_NAME"
    fi

    # clear unittest
    if [ -x "$UNITTEST" ]; then
        rm "$UNITTEST"
    fi

    cmake ..
    make -j16

    cp ./"$PROJECT_NAME" ./../../"$PROJECT_NAME"

    # core GTest
    if [ $unittest = true ]; then
        if [ -x "$UNITTEST" ]; then
            $UNITTEST
        fi
    fi
else
    error "UNKNOW PROJECT!"
fi
