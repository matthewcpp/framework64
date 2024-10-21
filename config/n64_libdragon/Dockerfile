# Build with: docker build -t matthewcpp/framework64-n64-libdragon .
# On Apple silicon: docker build -t matthewcpp/framework64-n64-libdragon --platform linux/amd64 .

FROM ubuntu:jammy

# install libdragon toolchain

ENV FW64_N64_LIBDRAGON_DOCKER_CONTAINER=1
ENV N64_INST=/opt/libdragon
ENV T3D_INST=/opt/tiny3d

ARG LIBDRAGON_TOOLCHAIN_URL="https://github.com/DragonMinded/libdragon/releases/download/toolchain-continuous-prerelease/gcc-toolchain-mips64-x86_64.deb"
ARG NODE_MAJOR_VERSION=20

# Libdragon - preview - 10/29/2024
ARG LIBDRAGON_GIT_REPO=https://github.com/DragonMinded/libdragon.git
ARG LIBDRAGON_GIT_COMMIT=e36e91ea4ede4f70472bbf0086689ff5f4cf4aa5

# Tiny3d - main - 10/29/2024
ARG TINY3D_GIT_REPO=https://github.com/HailToDodongo/tiny3d.git
ARG TINY3D_GIT_COMMIT=68842553bf330a91fc595a7c9803bfd198d3e468

RUN apt-get update && \
    apt-get install -y nano wget git g++ gdb-multiarch cmake ninja-build && \
    \
    cd /tmp && wget ${LIBDRAGON_TOOLCHAIN_URL} && apt-get install -y ./gcc-toolchain-mips64-x86_64.deb && \
    rm ./gcc-toolchain-mips64-x86_64.deb && \
    \
    apt-get install -y ca-certificates curl gnupg && \
    mkdir -p /etc/apt/keyrings && \
    curl -fsSL https://deb.nodesource.com/gpgkey/nodesource-repo.gpg.key | gpg --dearmor -o /etc/apt/keyrings/nodesource.gpg && \
    echo "deb [signed-by=/etc/apt/keyrings/nodesource.gpg] https://deb.nodesource.com/node_${NODE_MAJOR_VERSION}.x nodistro main" | tee /etc/apt/sources.list.d/nodesource.list && \
    apt-get update && apt-get install -y nodejs && \
    \
    rm -rf /var/lib/apt/lists/* 

RUN cd /tmp && git clone ${LIBDRAGON_GIT_REPO} && cd libdragon && git checkout ${LIBDRAGON_GIT_COMMIT} && \
    make libdragon tools && make install tools-install && \
    cd /opt && git clone ${TINY3D_GIT_REPO} && cd tiny3d && git checkout ${TINY3D_GIT_COMMIT} && ./build.sh