#include "App.h"
#include "connections/Client.h"
#include "connections/SocketConnection.h"
#include "connections/SslConnection.h"

#include <arpa/inet.h>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <netinet/in.h>
#include <spdlog/spdlog.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int create_socket(const int port) {
    sockaddr_in addr{};

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    const int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        spdlog::critical("Unable to create socket");
        exit(EXIT_FAILURE);
    }

    if (bind(sockfd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) < 0) {
        spdlog::critical("Unable to bind port {}", addr.sin_port);
        exit(EXIT_FAILURE);
    }

    if (listen(sockfd, 1) < 0) {
        spdlog::critical("Unable to listen");
        exit(EXIT_FAILURE);
    }

    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, nullptr, 0);
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, nullptr, 0);
    setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, nullptr, 0);

    fcntl(sockfd, F_SETFL, O_NONBLOCK);

    return sockfd;
}

bool App::is_ssl_enabled() const { return ssl_ctx != nullptr; }

void App::run(const int port) {
    sockfd = create_socket(port);

    int max_sd = sockfd;
    fd_set master_fds, read_fds;

    FD_ZERO(&master_fds);
    FD_ZERO(&read_fds);

    FD_SET(sockfd, &master_fds);

    std::map<int, std::unique_ptr<Client>> clients;

    spdlog::info("Server {} listening on port {}", sockfd, port);

    while (is_running) {
        read_fds = master_fds;

        const auto operation = select(max_sd + 1, &read_fds, nullptr, nullptr, nullptr);
        if (operation < 0) {
            spdlog::warn("Unable to select");
            continue;
        }

        if (FD_ISSET(sockfd, &read_fds)) {
            sockaddr_in addr{};
            uint len = sizeof(addr);

            const int new_socket = accept(sockfd, reinterpret_cast<struct sockaddr *>(&addr), &len);
            if (new_socket < 0) {
                spdlog::warn("Unable to accept");
                continue;
            }

            // Set new socket to non blocking
            if (fcntl(new_socket, F_SETFL, O_NONBLOCK) == -1) {
                spdlog::warn("Unable to fcntl");
                continue;
            }

            spdlog::trace("New client {} connected {}:{}", new_socket, inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));

            FD_SET(new_socket, &master_fds);
            if (new_socket > max_sd) {
                max_sd = new_socket;
            }

            // Create the client
            auto new_client_info = std::make_unique<Client>(new_socket, router);
            clients[new_socket] = std::move(new_client_info);
        }

        for (int i = 0; i <= max_sd; i++) {
            if (i != sockfd && FD_ISSET(i, &read_fds)) {
                // Retrieve the client
                if (auto it = clients.find(i); it != clients.end()) {
                    if (it->second->read() == DISCONNECTED) {
                        FD_CLR(i, &master_fds);
                        clients.erase(it);
                        break;
                    }
                }
            }
        }
    }
}

void App::handle_client(const int &client) const {
    if (is_ssl_enabled()) {
        SslConnection connection(client, ssl_ctx);

        if (!connection.handshake()) {
            connection.close_socket();
            return;
        }

        accept_connection(connection);
    } else {
        SocketConnection connection(client);
        accept_connection(connection);
    }
}

void App::accept_connection(Connection &connection) const {
    try {
        const auto request = connection.socket_read();
        Response response{};

        router.handle_request(request, response);

        spdlog::info("\"{} {}\" {}", request.get_method(), request.get_url().get_full_url(),
                     response.get_status_message());

        connection.write_socket(response.build());
    } catch (const std::runtime_error &e) {
    }
}

void App::close_socket() const {
    close(sockfd);

    if (ssl_ctx != nullptr) {
        SSL_CTX_free(ssl_ctx);
    }
    spdlog::info("Server closed!");
}

App &App::enable_ssl(const std::string &cert, const std::string &key) {
    init_openssl();
    ssl_ctx = create_context();
    configure_context(ssl_ctx, cert.c_str(), key.c_str());
    spdlog::info("SSL is enabled");
    return *this;
}
