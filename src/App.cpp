#include "App.h"

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <cstdio>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "connections/SocketConnection.h"
#include "connections/SslConnection.h"


int create_socket(const int port) {
    sockaddr_in addr{};

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    const int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Unable to create socket");
        exit(EXIT_FAILURE);
    }

    if (bind(sock, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) < 0) {
        perror("Unable to bind");
        exit(EXIT_FAILURE);
    }

    if (listen(sock, 1) < 0) {
        perror("Unable to listen");
        exit(EXIT_FAILURE);
    }

    return sock;
}

bool App::is_ssl_enabled() const {
    return this->ctx != nullptr;
}

void App::run(const int port) {
    this->sockfd = create_socket(port);

    std::cout << "Server listening on port " << port << std::endl;

    if (this->is_ssl_enabled()) {
        std::cout << "SSL enabled." << std::endl;
    }

    while (true) {
        sockaddr_in addr{};
        uint len = sizeof(addr);

        const int client = accept(sockfd, reinterpret_cast<struct sockaddr *>(&addr), &len);
        if (client < 0) {
            perror("Unable to accept");
            break;
        }

        if (this->is_ssl_enabled()) {
            SslConnection connection(client, this->ctx);
            if (!connection.is_ready) {
                connection.close_socket();
                continue;
            }
            this->accept_connection(connection);
        } else {
            SocketConnection connection(client);
            this->accept_connection(connection);
        }
    }
}

void App::accept_connection(Connection &connection) {
    const auto bytes = connection.socket_read();
    const auto request = Request::parse(bytes);

    if (const auto path = request.get_path(); this->routes.contains(path)) {
        Response response = this->routes[path](request);
        connection.write_socket(response.build());
    } else {
        auto response = Response();
        response.set_status_code(404);
        connection.write_socket(response.build());
    }

    connection.close_socket();
}

void App::close_socket() const {
    std::cout << "Closing server." << std::flush;
    close(sockfd);
}

void App::route(const std::string &path, const std::function<Response (const Request &)> &callback) {
    this->routes[path] = callback;
}

App& App::enable_ssl(const std::string &cert, const std::string &key) {
    init_openssl();
    this->ctx = create_context();
    configure_context(this->ctx, cert.c_str(), key.c_str());
    return *this;
}
