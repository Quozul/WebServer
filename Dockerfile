FROM alpine AS base

FROM base AS build

WORKDIR /usr/src/app

RUN apk --no-cache add cmake make gcc g++ openssl-dev spdlog-dev

COPY main.cpp CMakeLists.txt ./
COPY ./src ./src

RUN mkdir build && \
    cd build && \
    cmake -DCMAKE_BUILD_TYPE=Release .. && \
    make -j$(nproc) && \
    cp /usr/src/app/build/web_server_reboot /usr/bin/web_server_reboot

FROM base AS run

WORKDIR /app
RUN apk --no-cache add spdlog && \
    addgroup --system --gid 1001 app && \
    adduser --system --uid 1001 app

COPY --from=build /usr/bin/web_server_reboot /usr/bin/web_server_reboot

USER app
EXPOSE 8080

CMD ["/usr/bin/web_server_reboot"]
