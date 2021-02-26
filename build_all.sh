#!/usr/bin/env bash

ROOT=/etc/n64

examples=( "arcball" "audio" "billboard" "data" "lines" "minimal" "sprites")

for i in "${examples[@]}"
do
    cd examples/$i && make
    cd ..
done