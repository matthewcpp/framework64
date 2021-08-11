#!/usr/bin/env bash

ROOT=/etc/n64

examples=( "audio" "billboard" "bounding" "data" "lines" "minimal" "rendermodes" "sprites" "texparams")

for i in "${examples[@]}"
do
    pushd examples/$i
    make clean && make

    if [ $? -ne 0 ]; then
        exit 1
    fi

    popd
done