#!/bin/sh

for f in tests/*.c
do
    ./run-test.sh "$f"
done
