# Web Server

> A web server written in C++ with TLS support with an API inspired by the Express.js framework.

This software is not yet ready for production use.

## Build

```sh
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)
```

## Run

### Configuration file

The web server searches for a file named `config` in the current working directory.

The configuration file is optional but can be used to configure the following values:

```yaml
port: 8080 # defaults to "8080"
log_level: info # defaults to "info"
cert: cert.pem
key: key.pem
```

All parameters are optional. If cert or key is not provided, TLS will be disabled.

#### Logging level

Available logging levels are:

- `trace`
- `debug`
- `info` (default)
- `warn`
- `err`
- `critical`
- `off`

### Enabling TLS

If you want to generate a self-signed certificate, you can use the following command:

```sh
openssl req -x509 -newkey rsa:4096 -keyout key.pem -out cert.pem -days 365 -nodes
./build/web_server_reboot
```

Modify the config file with the path to your certificate and key:

```yaml
cert: cert.pem
key: key.pem
```
