# Build with: docker build -t matthewcpp/framework64-n64-libultra .
# On Apple silicon: docker build -t matthewcpp/framework64_n64_libultra --platform linux/amd64 .

from matthewcpp/n64-modern-sdk

ARG NODE_MAJOR_VERSION=20
ARG CMAKE_VERSION=3.29.4
ARG CMAKE_ARCHIVE=cmake-${CMAKE_VERSION}-linux-x86_64.tar.gz

ENV FW64_N64_LIBULTRA_DOCKER_CONTAINER=1
ENV PATH=/opt/cmake-3.29.4-linux-x86_64/bin:${PATH}

RUN apt-get update && apt-get remove -y cmake && cd /opt && \
    wget https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/${CMAKE_ARCHIVE} && tar -xzvf ${CMAKE_ARCHIVE} && rm ${CMAKE_ARCHIVE} && \
    apt-get install -y ca-certificates curl gnupg && \
    mkdir -p /etc/apt/keyrings && \
    curl -fsSL https://deb.nodesource.com/gpgkey/nodesource-repo.gpg.key | gpg --dearmor -o /etc/apt/keyrings/nodesource.gpg && \
    echo "deb [signed-by=/etc/apt/keyrings/nodesource.gpg] https://deb.nodesource.com/node_${NODE_MAJOR_VERSION}.x nodistro main" | tee /etc/apt/sources.list.d/nodesource.list && \
    apt-get update && apt-get install -y nodejs && \
    apt-get install -y libsndfile1 wine && wineconsole --help && \
    rm -rf /var/lib/apt/lists/* 


#from ubuntu:jammy

#ENV FW64_N64_LIBULTRA_DOCKER_CONTAINER=1

#ENV PATH=/opt/crashsdk/bin:/etc/n64/usr/sbin:${PATH}
#ENV N64_LIBGCCDIR=/opt/crashsdk/lib/gcc/mips64-elf/11.2.0

#WORKDIR /opt/crashsdk

#COPY modernsdk /modernsdk

#setup system libs
#RUN apt-get update && \
#    # install package dependencies
#    dpkg --add-architecture i386 && apt-get update && \
#    DEBIAN_FRONTEND="noninteractive" apt-get install -y wget gnupg software-properties-common unzip p7zip cabextract curl nano build-essential gdb-multiarch git flex bison pkg-config && \
#    echo "deb [trusted=yes] https://crashoveride95.github.io/apt ./" > /etc/apt/sources.list.d/crashoveride95.list && apt-get update && \
#    apt-get install -y lib32z1 libc6:i386 && \
#    apt-get install -y root-compatibility-environment && \
#    apt-get install -y n64sdk makemask libnustd libnusys libnaudio libmus n64-demos nusys-demos rsp-tools && \
#    rm -rf /var/lib/apt/lists/* 