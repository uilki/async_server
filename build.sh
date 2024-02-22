#!/bin/bash

PROJECT="VUSERVER"

BUILD_DIR=$(pwd)/../${PROJECT}_build

GEN=$(which ninja)
CXX=$(which g++)

builds=( "client" "server" "all" )
build_types=( [0]="Debug" [1]="Release" [2]="RelWithDebInfo" [3]="MinSizeRel" )

SCRIPT_NAME=$0

usage()
{
    echo "Usage:"
    echo "$SCRIPT_NAME <target> [build type]" 
    echo -en "\tSupported targets: "
    for target in ${builds[@]}; do
        echo -n "\"${target}\" "
    done
    echo
    echo -en "\tSupported build types: "
    for type in "${build_types[@]}"; do
        echo -n "\"${type}\" "
    done
    echo "(default = ${build_types[0]})"
    exit 1
}

if [[ $# < 1 ]]; then
    echo "Wrong number of arguments"
    usage
fi

TARGET=$1

isValidTarget()
{
    for target in "${builds[@]}"; do
        if [ "${target}" = "${TARGET}" ]; then
            return 1
        fi
    done
    return 0
}

isValidTarget
targetValid=$?
if [ $targetValid = 0 ]; then
    usage
fi

BUILD_TYPE=$2

buildType()
{
    for type in "${build_types[@]}"; do
        if [ "${type}" = "${BUILD_TYPE}" ]; then
            return
        fi
    done
    BUILD_TYPE=${build_types[0]}
    return
}

buildType

BUILD_DIR=${BUILD_DIR}--${BUILD_TYPE}
BINARY_OUT_DIR=${BUILD_DIR}/out

RUNTIME_DIR=${BINARY_OUT_DIR}/bin
LIB_DIR=${BINARY_OUT_DIR}/lib
ARC_DIR=${BINARY_OUT_DIR}/lib

if [ -z "$GEN"  ]; then
    echo "Ninja generator isn't installed.\nInstall ninja-build package..."
    sudo apt-get install -y ninja-build
    GEN=$(which ninja)
fi

if [ -z "$CXX"  ]; then
echo "g++ isn't installed.\nInstall g++ package..."
    sudo apt-get install g++
    CXX=$(which g++)
fi

echo "Running cmake..."

cmake \
    -G "Ninja" \
    -DNAME=${PROJECT} \
    -DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
    -DCMAKE_CXX_COMPILER=${CXX} \
    -DCMAKE_LIBRARY_OUTPUT_DIRECTORY=${LIB_DIR} \
    -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=${RUNTIME_DIR} \
    -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY=${ARC_DIR} \
    -B${BUILD_DIR} \
    -S.

echo "Running ninja..."
echo "Build target ${TARGET}"
cd ${BUILD_DIR} && ninja "${TARGET}"
