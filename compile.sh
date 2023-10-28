#!/bin/bash

while getopts ":p:" opt; do
    case $opt in
        p)
            project="$OPTARG"
            project=${project%/}
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

# compile
if [ -d "$PROJECT_NAME" ]; then
    cd "$PROJECT_NAME"
    if [ ! -d build ]; then
        mkdir build
    fi
    cd build
    cmake ..
    make -j16

    cp ./"$PROJECT_NAME" ./../../"$PROJECT_NAME"
else
    error "UNKNOW PROJECT!"
fi
