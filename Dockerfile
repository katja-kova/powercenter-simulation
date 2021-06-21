FROM docker.io/ubuntu:latest AS builder
#preparing(envs, installation of prerequisites and creating directories)
ENV MY_INSTALL_DIR="/app/.local"
ENV PATH="/app/.local/bin:$PATH"
RUN apt-get clean && \
    apt-get update && \
    apt-get dist-upgrade -y
RUN DEBIAN_FRONTEND=noninteractive apt-get -y --no-install-recommends install tzdata && \
    apt-get install -y --no-install-recommends ca-certificates cmake build-essential libtool autoconf pkg-config git
RUN mkdir -p /grpc /app/.local
#installing grpc + protobuf
RUN git clone --recurse-submodules -b v1.37.1 https://github.com/grpc/grpc
RUN mkdir -p /grpc/cmake/build /grpc/third_party/abseil-cpp/cmake/build
WORKDIR /grpc/cmake/build
RUN cmake -DgRPC_INSTALL=ON -DgRPC_BUILD_TESTS=OFF -DCMAKE_INSTALL_PREFIX=$MY_INSTALL_DIR ../..; make -j 16; make install
WORKDIR /grpc/third_party/abseil-cpp/cmake/build
RUN cmake -DCMAKE_INSTALL_PREFIX=$MY_INSTALL_DIR -DCMAKE_POSITION_INDEPENDENT_CODE=TRUE ../..; make -j 16; make install
#cleanup
RUN rm -rf /grpc

FROM docker.io/ubuntu:latest AS base
ENV MY_INSTALL_DIR="/app/.local"
ENV PATH="/app/.local/bin:$PATH"
RUN apt-get clean && apt-get update && apt-get dist-upgrade -y
RUN DEBIAN_FRONTEND=noninteractive apt-get -y --no-install-recommends install tzdata && \
    apt-get install make cmake g++ pkg-config libmosquitto-dev libmosquittopp-dev -y
RUN mkdir -p /app/cpp/src
COPY --from=builder /app/.local /app/.local

FROM docker.io/eclipse-mosquitto:latest AS mqtt
COPY mosquitto.conf mosquitto/config/mosquitto.conf
