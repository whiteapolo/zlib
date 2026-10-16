#!/bin/sh

if [ -z "$1" ]
then
    echo "need test name. aborting."
fi

if [ ! -e "$1" ]
then
    echo "test doesnt exists. aborting."
fi

cc "$1" -o tmp ../build/libzatar.a
./tmp > "$1.stdout"
rm tmp
