FROM ubuntu:18.04

RUN set -x \
    && apt-get update \
    && apt-get install -y libreadline-dev \
    && apt-get install -y build-essential \
    && apt-get install -y git wget \
    && apt-get install -y zlib1g-dev \
    && apt-get clean \
    && mkdir -p /Software \
    && wget -O "/Software/cmake-3.14.5-Linux-x86_64.tar.gz" "https://github.com/Kitware/CMake/releases/download/v3.14.5/cmake-3.14.5-Linux-x86_64.tar.gz" \
    && cd /Software \
    && tar xvf "cmake-3.14.5-Linux-x86_64.tar.gz" \
    && rm -rf "/Software/cmake-3.14.5-Linux-x86_64.tar.gz" \
    && mkdir -p /sources \
    && cd /sources \
    && git clone https://github.com/dibyendumajumdar/ravi.git \
    && cd /sources/ravi \
    && mkdir build \
    && cd build \
    && /Software/cmake-3.14.5-Linux-x86_64/bin/cmake -DCMAKE_INSTALL_PREFIX=/Software/ravi -DCMAKE_BUILD_TYPE=Release .. \
    && make install \
    && rm -rf /Software/cmake-3.14.5-Linux-x86_64 \
    && rm -rf /sources \
    && apt-get autoremove \
    && apt-get remove -y --purge git wget build-essential \
    && apt-get clean

ENV PATH /Software/ravi/bin:${PATH}

