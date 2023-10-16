#!/bin/bash

while getopts ":p:" opt; do
    case $opt in
        p)
            project="$OPTARG"
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
if [ -d "./core/3rd/uWebSockets" ]; then
    cd ./core/3rd/uWebSockets
    git submodule update --init
    cd ../../../
else
    error "Cannot found ./core/3rd/uWebSockets"
fi

# compile
if [ -d "$PROJECT_NAME" ]; then
    cd "$PROJECT_NAME"
    if [ ! -d build ]; then
        mkdir build
    fi
    cd build
    cmake ..
    make -j16
else
    error "UNKNOW PROJECT!"
fi
