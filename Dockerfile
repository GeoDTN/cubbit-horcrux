FROM docker.io/library/debian:bookworm-slim
#FROM ubuntu:24.04
WORKDIR /srv/horcrux_xencr_decr/
RUN apt-get update && apt-get install gcc build-essential libssl-dev wget -y
RUN wget https://www.openssl.org/source/openssl-3.0.0.tar.gz && tar -xzvf openssl-3.0.0.tar.gz

RUN cd openssl-3.0.0 && ./config enable-ssl-trace && make && make install
RUN wget https://github.com/Kitware/CMake/releases/download/v3.24.1/cmake-3.24.1-Linux-x86_64.sh \
      -q -O /tmp/cmake-install.sh \
      && chmod u+x /tmp/cmake-install.sh \
      && mkdir /opt/cmake-3.24.1 \
      && /tmp/cmake-install.sh --skip-license --prefix=/opt/cmake-3.24.1 \
      && rm /tmp/cmake-install.sh \
      && ln -s /opt/cmake-3.24.1/bin/* /usr/local/bin
RUN apt-get update && apt-get install -y \
 xz-utils \
 curl \
 && rm -rf /var/lib/apt/lists/*
RUN curl -SL https://github.com/llvm/llvm-project/releases/download/llvmorg-10.0.0/clang+llvm-10.0.0-x86_64-linux-gnu-ubuntu-18.04.tar.xz \
 | tar -xJC . && \
 mv clang+llvm-10.0.0-x86_64-linux-gnu-ubuntu-18.04 clang_10 && \
 echo ‘export PATH=/clang_10/bin:$PATH’ >> ~/.bashrc && \
 echo ‘export LD_LIBRARY_PATH=/clang_10/lib:$LD_LIBRARY_PATH’ >> ~/.bashrc
#start the container from bash
CMD [ “/bin/bash” ]
ENV LD_LIBRARY_PATH /usr/local/lib/:/usr/local/lib64/

ARG   INFO_PROJECT=""
ARG   INFO_VERSION=""
ARG   INFO_BRANCH=""
ARG   INFO_COMMIT=""
ARG   INFO_NAMESPACE=""
ARG   INFO_URL=""

LABEL Info.Project="$INFO_PROJECT"
LABEL Info.Version="$INFO_VERSION"
LABEL Info.Branch="$INFO_BRANCH"
LABEL Info.Commit="$INFO_COMMIT"
LABEL Info.Namespace="$INFO_NAMESPACE"
LABEL Info.Url="$INFO_URL"
COPY ./build/horcrux   /srv/horcrux_xencr_decr/
EXPOSE 22

ENTRYPOINT ["./horcrux"]
