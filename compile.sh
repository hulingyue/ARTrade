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

del() {
    if [ -x $1 ]; then
        rm $1
    fi
}

FILE_NAME="$0"
PROJECT_NAME="$project"

# download third-party libraries
git submodule update --init

# GTest
UNITTEST='./core/unittest'

path_exists=true
# compile
if [ -d "$PROJECT_NAME" ]; then
    cd "$PROJECT_NAME"
elif [ -d "exchange/crypto/$PROJECT_NAME" ]; then
    cd "exchange/crypto/$PROJECT_NAME"
elif [ -d "exchange/finance/$PROJECT_NAME" ]; then
    cd "exchange/finance/$PROJECT_NAME"
else
    error "UNKNOW PROJECT!"
    path_exists=true
fi

if [ path_exists ]; then
    if [ ! -d build ]; then
        mkdir build
    fi
    cd build

    # clean executation
    del $PROJECT_NAME
    del "./../$PROJECT_NAME"

    # clear unittest
    del $UNITTEST

    cmake ..
    make -j16

    info $PROJECT_NAME
    if [ -x "$PROJECT_NAME" ]; then
        cp ./"$PROJECT_NAME" ./../../"$PROJECT_NAME"
    else
        error "compile failure!"
        return
    fi

    # core GTest
    if [ $unittest = true ]; then
        if [ -x "$UNITTEST" ]; then
            $UNITTEST
        fi
    fi
fi