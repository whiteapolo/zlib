#!/bin/sh

if [ -z "$1" ]
then
    echo "need test name"
fi

make -C .. debug
cc "$1" -o tmp ../build/libzatar.a
result="$(./tmp)"
rm tmp

RED='\033[0;31m'
GREEN='\033[0;32m'
RESET='\033[0m'

if [ "$result" == "$(cat $1.stdout)" ]
then
    echo -e "[${GREEN}SUCCESS${RESET}] $1"
else
    echo -e "[${RED}FAILED${RESET}] $1"
fi
