#!/bin/sh
# Copyright 2019-2022, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0
# Author: Ryan Pavlik <ryan.pavlik@collabora.com>

# Formats all the CMake files in this project

set -e

CMAKE_FORMAT=cmake-format
if ! command -v ${CMAKE_FORMAT} > /dev/null; then
        echo "cmake-format not found, do python3 -m pip install cmakelang" 1>&2
        exit 1
fi
(
    echo "${CMAKE_FORMAT}"
    "${CMAKE_FORMAT}" -v

    cd "$(dirname $0)/.."
    find \
        CMakeLists.txt \
        doc \
        src/xrt \
        src/external/CMakeLists.txt \
        src/CMakeLists.txt \
        tests \
        \( -name "CMakeLists.txt" -o -name "*.cmake" \) \
        ! -name ".cmake" \
        -print0 | \
        xargs -0 "${CMAKE_FORMAT}" -c "$(pwd)/.cmake-format.py" -i
)
