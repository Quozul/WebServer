# Web Server

> [!WARNING]
> This software is not yet ready for production use as it lacks security features.

A web server written in C++ with TLS support, inspired by the Express.js framework.

## Build Instructions

### Dependencies

To build this project, you need to install the following libraries:

- **openssl**: For SSL/TLS support.
- **spdlog**: For logging functionality.

If you want to run the tests, you must also install the **gtest** library, though it's not required for building the
binary. You may also need the typical development tools like `gcc`, `g++` and `cmake`.

#### On Ubuntu:

```shell
sudo apt install -y cmake libgtest-dev libspdlog-dev
```

### Building the Project

1. Create a build directory:

    ```sh
    mkdir build
    cd build
    ```

2. Configure the project using `cmake`:

    ```sh
    cmake -DCMAKE_BUILD_TYPE=Release ..
    ```

3. Build the project with `make`:

    ```sh
    make -j$(nproc)
    ```

## Running the Server

### Docker Support

Docker images are built from the `master` branch and published on `ghcr.io`. You can find an
example [Dockerfile](/Dockerfile) and [docker-compose.yml](/docker-compose.yml) in this repository to help you run the
server in a container.

### Configuration File

The web server will look for a file named `config` in the current working directory. This file is optional but can be
used to configure the following parameters:

```yaml
port: 8080          # Defaults to "8080"
log_level: info     # Defaults to "info"
cert: cert.pem      # Path to your SSL certificate
key: key.pem        # Path to your SSL key
```

If `cert` or `key` are not provided, TLS will be disabled.

#### Logging Levels

The available logging levels are:

- `trace`
- `debug`
- `info` (default)
- `warn`
- `err`
- `critical`
- `off`

### Enabling TLS

To enable TLS support, you need an SSL certificate. You can generate a self-signed certificate using the following
command:

```sh
openssl req -x509 -newkey rsa:4096 -keyout key.pem -out cert.pem -days 365 -nodes
```

Modify the `config` file to specify the paths to your certificate and key:

```yaml
cert: cert.pem
key: key.pem
```

After generating and modifying the `config` file the certificate, run the server:

```sh
./build/web_server_reboot
```

## Contributing

Contributions are welcome! If you encounter any issues or have suggestions for improvement, please submit an issue or
pull request on GitHub. Make sure to follow the existing code style and include relevant tests.

1. Fork the repository.
2. Create a new branch `git checkout -b <branch-name>`.
3. Make changes and commit `git commit -m 'Add some feature'`.
4. Push to your fork `git push origin <branch-name>`.
5. Submit a pull request.
