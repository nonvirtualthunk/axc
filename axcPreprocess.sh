#!/bin/bash

MAINDIR=`dirname $0`
echo "$MAINDIR/axcMetaGenerator"
if [ -a "$MAINDIR/axcMetaGenerator" ]
    then
        for var in "$@"
        do
            "$MAINDIR/axcMetaGenerator" "$MAINDIR/$var"
        done
else
    echo "No axcMetaGenerator"
    echo $0
fi