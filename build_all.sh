#!/usr/bin/env bash

ROOT=/etc/n64

npm install
node tools/prepare_assets.js

examples=( "arcball" "billboard" "lines" "sprites")

for i in "${examples[@]}"
do
    cd examples/$i && make
    cd ..
done