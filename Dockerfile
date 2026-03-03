FROM alpine AS builder

RUN apk add --no-cache \
    build-base \
    cmake \
    xxd \
    curl-dev \
    openssl-dev \
    libmicrohttpd-dev \
    nlohmann-json

COPY . /src
WORKDIR /src
RUN cmake -B build -D CMAKE_BUILD_TYPE=MinSizeRel -D WITHOUT_TEST=ON
RUN cmake --build build
RUN cmake --install build

FROM alpine AS image

RUN apk add --no-cache \
    libstdc++ \
    curl \
    openssl \
    libmicrohttpd
COPY --from=builder /usr/local/bin/container-ui /usr/local/bin/container-ui

WORKDIR /
ENTRYPOINT ["/usr/local/bin/container-ui"]
