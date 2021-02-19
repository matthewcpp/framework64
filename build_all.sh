#!/usr/bin/env bash

ROOT=/etc/n64

examples=( "arcball" "billboard" "data" "lines" "minimal" "sprites")

for i in "${examples[@]}"
do
    cd examples/$i && make
    cd ..
done