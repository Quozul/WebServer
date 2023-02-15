FROM ubuntu AS build

WORKDIR /app

RUN apt update && apt install -y build-essential liblua5.4-dev libssl-dev cmake git

COPY . .

RUN cmake CMakeLists.txt
RUN cmake --build . --target install -- -j $(nproc)

FROM alpine AS run

WORKDIR /app

COPY --from=build /app/WebServer ./

RUN apk add --no-cache lua5.4 openssl

COPY docker/etc /etc
COPY docker/www /var/www

RUN openssl req  -nodes -new -x509 -keyout /app/server.key -out /app/server.cert \
    -subj "/C=UK/ST=Warwickshire/L=Leamington/O=OrgName/OU=IT Department/CN=example.com"

CMD ./WebServer
