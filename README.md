# Web Server

A web server written in C++ with SSL support.
With an API inspired by the Express.js framework.

## Build

```sh
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)
```

## Run

### Configuration file

The configuration file is optional but can be used to configure the following values:

```yaml
port: 443
cert: cert.pem
key: key.pem
```

If you want to generate a self-signed certificate, you can use the following command:
```sh
openssl req -x509 -newkey rsa:4096 -keyout key.pem -out cert.pem -days 365 -nodes
./build/web_server_reboot
```
