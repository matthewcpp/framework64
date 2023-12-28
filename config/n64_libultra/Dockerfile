from ubuntu:jammy

ENV PATH=/opt/crashsdk/bin:/etc/n64/usr/sbin:${PATH}
ENV N64_LIBGCCDIR=/opt/crashsdk/lib/gcc/mips64-elf/11.2.0

WORKDIR /opt/crashsdk

RUN apt-get update && \
    # install package dependencies
    dpkg --add-architecture i386 && apt-get update && \
    DEBIAN_FRONTEND="noninteractive" apt-get install -y wget gnupg software-properties-common unzip p7zip cabextract curl nano build-essential gdb-multiarch git flex bison cmake pkg-config && \

    # configure crash's modern SDK
    echo "deb [trusted=yes] https://crashoveride95.github.io/apt ./" > /etc/apt/sources.list.d/crashoveride95.list && apt-get update && \
    apt-get install -y lib32z1 libc6:i386 && \
    apt-get install -y binutils-mips-n64 gcc-mips-n64 newlib-mips-n64 root-compatibility-environment && \
    apt-get install -y n64sdk makemask libnustd libnusys libnaudio libmus n64-demos nusys-demos rsp-tools && \
    mkdir /src /dest /job /fw64 && \
    rm -rf /var/lib/apt/lists/* 