FROM alpine

WORKDIR /src/app

RUN apk --no-cache add cmake make gcc g++ openssl-dev spdlog-dev

COPY . .

RUN mkdir build && \
    cd build && \
    cmake -DCMAKE_BUILD_TYPE=Release .. && \
    make -j$(nproc)

CMD ["./build/web_server_reboot"]
