# Build with: docker build -t matthewcpp/framework64_n64_libdragon .
# On Apple silicon: docker build -t matthewcpp/framework64_n64_libdragon --platform linux/amd64 .

FROM ubuntu:resolute

# install libdragon toolchain

ENV FW64_DOCKER_CONTAINER="n64_libdragon"
ENV N64_INST=/opt/libdragon
ENV T3D_INST=/opt/tiny3d

ARG LIBDRAGON_TOOLCHAIN_URL="https://github.com/DragonMinded/libdragon/releases/download/toolchain-continuous-prerelease/gcc-toolchain-mips64-x86_64.deb"
ARG NODE_MAJOR_VERSION=22

# Libdragon - preview - 07/02/2026
ARG LIBDRAGON_GIT_REPO=https://github.com/DragonMinded/libdragon.git
ARG LIBDRAGON_GIT_COMMIT=a21e58869eec8e944593aa07ebdeeaadc88e3319

# Tiny3d - main - 06/18/2026
ARG TINY3D_GIT_REPO=https://github.com/HailToDodongo/tiny3d.git
ARG TINY3D_GIT_COMMIT=7f5773f64a0cc8709d9126aa52da16d07c696cc2

# framework64 for package.json only - main - 06/29/2026
ARG FRAMEWORK64_GIT_REPO="https://raw.githubusercontent.com/matthewcpp/framework64"
ARG FRAMEWORK64_GIT_COMMIT=90e159c40242125ddf85f7aadc7b6abc10f761ae

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
    cd /opt && git clone ${TINY3D_GIT_REPO} && cd tiny3d && git checkout ${TINY3D_GIT_COMMIT} && make && make install && \
    mkdir /opt/framework64 && cd /opt/framework64 && wget ${FRAMEWORK64_GIT_REPO}/${FRAMEWORK64_GIT_COMMIT}/package.json && npm install
