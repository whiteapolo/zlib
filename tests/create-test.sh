#!/bin/sh

if [ -z "$1" ]
then
    echo "need test name. aborting."
    exit
fi

if [ -e "$1" ]
then
    echo "test already exists. aborting."
    exit
fi

cp tests/test.example.c "$1"
