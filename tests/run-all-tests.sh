#!/bin/sh

set -e
make -C .. dev

for f in tests/*.c
do
    ./run-test.sh "$f"
done
