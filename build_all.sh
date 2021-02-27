#!/usr/bin/env bash

ROOT=/etc/n64

examples=( "audio" "billboard" "data" "lines" "minimal" "rendermodes" "sprites")

for i in "${examples[@]}"
do
    cd examples/$i && make
    cd ..
done