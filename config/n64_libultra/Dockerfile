# Build with: docker build -t matthewcpp/framework64_n64_libultra .
# On Apple silicon: docker build -t matthewcpp/framework64_n64_libultra --platform linux/amd64 .

from matthewcpp/n64-modern-sdk

ENV CMAKE_VERSION=3.28.1
ENV NODE_MAJOR_VERSION=20
ENV NINJA_VERSION=v1.11.1
ENV FW64_N64_LIBULTRA_DOCKER_CONTAINER=1


RUN apt-get update && apt-get remove -y cmake && apt-get install -y unzip && \

# Download and setup CMake
cd /opt && \
wget https://github.com/Kitware/CMake/releases/download/v$CMAKE_VERSION/cmake-$CMAKE_VERSION-linux-x86_64.tar.gz && \
tar -xzvf cmake-$CMAKE_VERSION-linux-x86_64.tar.gz && \
rm cmake-$CMAKE_VERSION-linux-x86_64.tar.gz && \
ln -s /opt/cmake-$CMAKE_VERSION-linux-x86_64/bin/cmake /usr/bin/cmake && \

# Insall Node
apt-get install -y ca-certificates curl gnupg && \
mkdir -p /etc/apt/keyrings && \
curl -fsSL https://deb.nodesource.com/gpgkey/nodesource-repo.gpg.key | gpg --dearmor -o /etc/apt/keyrings/nodesource.gpg && \
echo "deb [signed-by=/etc/apt/keyrings/nodesource.gpg] https://deb.nodesource.com/node_${NODE_MAJOR_VERSION}.x nodistro main" | tee /etc/apt/sources.list.d/nodesource.list && \
apt-get update && apt-get install -y nodejs && \

# Download and setup Ninja
wget https://github.com/ninja-build/ninja/releases/download/$NINJA_VERSION/ninja-linux.zip && \
unzip ninja-linux.zip -d ninja-$NINJA_VERSION && \
rm ninja-linux.zip && \
ln -s /opt/ninja-$NINJA_VERSION/ninja /usr/bin/ninja && \

rm -rf /var/lib/apt/lists/*

# Note: Never versions of Modern SDK seem to have problems with CEN64 Debugging. When that is fixed the code below can be used to build the container from scratch

# from ubuntu:jammy
# 
# ENV FW64_N64_LIBULTRA_DOCKER_CONTAINER=1
# 
# ENV PATH=/opt/crashsdk/bin:/etc/n64/usr/sbin:${PATH}
# ENV N64_LIBGCCDIR=/opt/crashsdk/lib/gcc/mips64-elf/11.2.0
# 
# WORKDIR /opt/crashsdk
# 
# RUN apt-get update && \
#     # install package dependencies
#     dpkg --add-architecture i386 && apt-get update && \
#     DEBIAN_FRONTEND="noninteractive" apt-get install -y wget gnupg # software-properties-common unzip p7zip cabextract curl nano build-essential # gdb-multiarch git flex bison cmake pkg-config && \
# 
#     # configure crash's modern SDK
#     echo "deb [trusted=yes] https://crashoveride95.github.io/apt ./" > /etc/apt/# sources.list.d/crashoveride95.list && apt-get update && \
#     apt-get install -y lib32z1 libc6:i386 && \
#     apt-get install -y binutils-mips-n64 gcc-mips-n64 newlib-mips-n64 # root-compatibility-environment && \
#     apt-get install -y n64sdk makemask libnustd libnusys libnaudio libmus n64-demos # nusys-demos rsp-tools && \
#     mkdir /src /dest /job /fw64 && \
#     rm -rf /var/lib/apt/lists/* # 