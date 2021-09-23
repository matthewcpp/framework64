#!/usr/bin/env bash

if [ ! -d /src/build_n64 ]; then
  echo "build_n64 directory does not exist.  Use 'npm run prepare-n64-assets' and then re-run this script"
fi

cd /src/build_n64
cmake -DFW64_PLATFORM_N64=ON ..
cmake --build .