#include "App.h"

#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <future>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

#include "connections/SocketConnection.h"
#include "connections/SslConnection.h"
#include "tracing.h"

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
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, nullptr, 0);
    setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, nullptr, 0);

    fcntl(sockfd, F_SETFL, O_NONBLOCK);

    return sockfd;
}

bool App::is_ssl_enabled() const { return this->ssl_ctx != nullptr; }

void App::run(const int port) {
    this->sockfd = create_socket(port);

    tracing::info("Server listening on port {}.", port);

    std::vector<std::future<void>> pending_futures;

    fd_set read_set;
    FD_ZERO(&read_set);
    FD_SET(sockfd, &read_set);

    while (this->is_running) {
        sockaddr_in addr{};
        uint len = sizeof(addr);

        select(sockfd + 1, &read_set, nullptr, nullptr, nullptr);

        if (FD_ISSET(sockfd, &read_set)) {
            const int client = accept(
                sockfd, reinterpret_cast<struct sockaddr *>(&addr), &len);
            if (client < 0) {
                tracing::warn("Unable to accept");
                break;
            }

            auto new_future =
                std::async(std::launch::async, &App::handle_client, this,
                           std::ref(client));
            pending_futures.push_back(std::move(new_future));
        }

        std::erase_if(pending_futures, [](const auto &future) {
            return future.wait_for(std::chrono::seconds(0)) ==
                   std::future_status::ready;
        });
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
            const auto request = connection.socket_read();

            tracing::info("{} {}", request.get_method(),
                          request.get_url().get_full_url());

            Response response = handle_request(request);
            connection.write_socket(response.build());
        } catch (UndefinedRoute &e) {
            auto response = Response{};
            response.set_status_code(404);
            connection.write_socket(response.build());
        } catch (const std::runtime_error &e) {
            break;
        }
    }

    connection.close_socket();
}

void App::close_socket() const {
    tracing::info("Closing server.");
    close(sockfd);

    if (this->ssl_ctx != nullptr) {
        SSL_CTX_free(this->ssl_ctx);
    }
    tracing::info("Server closed!");
}

void App::route(const std::string &path, const Handler &callback) {
    this->routes[path] = callback;
}

Response App::handle_request(const Request &request) const {
    if (const auto path = request.get_url().get_path();
        this->routes.contains(path)) {
        return this->routes.at(path)(request);
    }

    throw UndefinedRoute{};
}

App &App::enable_ssl(const std::string &cert, const std::string &key) {
    init_openssl();
    this->ssl_ctx = create_context();
    configure_context(this->ssl_ctx, cert.c_str(), key.c_str());
    tracing::info("SSL enabled");
    return *this;
}
