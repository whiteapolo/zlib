#!/bin/sh

set -e
make -C .. debug

for f in tests/*.c
do
    ./run-test.sh "$f"
done
