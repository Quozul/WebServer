## Build

```sh
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j
```

## Run

```sh
openssl req -x509 -newkey rsa:4096 -keyout key.pem -out cert.pem -days 365 -nodes
./build/web_server_reboot
```
