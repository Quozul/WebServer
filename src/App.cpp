#include "App.h"

#include <cstdio>
#include <cstdlib>
#include <poll.h>
#include <unistd.h>
#include <format>
#include <future>
#include <vector>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "tracing.h"
#include "connections/SocketConnection.h"
#include "connections/SslConnection.h"


int create_socket(const int port) {
    sockaddr_in addr{};

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    const int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        tracing::error("Unable to create socket");
        exit(EXIT_FAILURE);
    }

    if (bind(sockfd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) < 0) {
        tracing::error("Unable to bind");
        exit(EXIT_FAILURE);
    }

    if (listen(sockfd, 1) < 0) {
        tracing::error("Unable to listen");
        exit(EXIT_FAILURE);
    }

    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, nullptr, 0);

    return sockfd;
}

bool App::is_ssl_enabled() const {
    return this->ssl_ctx != nullptr;
}

void App::run(const int port) {
    this->sockfd = create_socket(port);

    tracing::info("Server listening on port {}.", port);

    std::vector<std::future<void>> pending_futures;

    while (true) {
        sockaddr_in addr{};
        uint len = sizeof(addr);

        const int client = accept(sockfd, reinterpret_cast<struct sockaddr *>(&addr), &len);
        if (client < 0) {
            tracing::warn("Unable to accept");
            break;
        }

        auto future = std::async(std::launch::async, &App::handle_client, this, std::ref(client));
        pending_futures.push_back(std::move(future));
    }
}

void App::handle_client(const int &client) const {
    if (this->is_ssl_enabled()) {
        SslConnection connection(client, this->ssl_ctx);

        if (!connection.handshake()) {
            connection.close_socket();
            return;
        }

        this->accept_connection(connection);
    } else {
        SocketConnection connection(client);
        this->accept_connection(connection);
    }
}

void App::accept_connection(Connection &connection) const {
    while (true) {
        try {
            const auto bytes = connection.socket_read();
            const auto request = Request::parse(bytes);

            tracing::info("{} {}", request.get_method(), request.get_full_url());

            if (const auto path = request.get_path(); this->routes.contains(path)) {
                Response response = this->routes.at(path)(request);
                connection.write_socket(response.build());
            } else {
                auto response = Response();
                response.set_status_code(404);
                connection.write_socket(response.build());
            }

            if (request.get_header("connection") != "keep-alive") {
                tracing::info("Connection is not keep-alive. Closing socket.");
                break;
            }
        } catch (const std::runtime_error &e) {
            tracing::info("Connection closed.");
            break;
        }
    }

    connection.close_socket();
}

void App::close_socket() const {
    tracing::info("Closing server.");
    close(sockfd);
}

void App::route(const std::string &path, const std::function<Response (const Request &)> &callback) {
    this->routes[path] = callback;
}

App &App::enable_ssl(const std::string &cert, const std::string &key) {
    init_openssl();
    this->ssl_ctx = create_context();
    configure_context(this->ssl_ctx, cert.c_str(), key.c_str());
    tracing::info("SSL enabled");
    return *this;
}
