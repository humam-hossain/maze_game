#!/bin/sh

set -xe
CFLAGS="-Wall -Wextra"

mkdir -p ./bin/
g++ $CFLAGS -o ./bin/main ./src/main.cpp
g++ $CFLAGS -o ./bin/test ./src/test.cpp