#include "App.h"

#include "clients/SocketClient.h"
#include "clients/SslClient.h"
#include "event_loops/EpollEventLoop.h"

#include <cstdlib>
#include <fcntl.h>
#include <netinet/in.h>
#include <spdlog/spdlog.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>
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

    int optval = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
        spdlog::critical("Failed to set SO_REUSEADDR: {}", strerror(errno));
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval)) < 0) {
        spdlog::critical("Failed to set SO_REUSEPORT: {}", strerror(errno));
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    if (setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval)) < 0) {
        spdlog::critical("Failed to set SO_KEEPALIVE: {}", strerror(errno));
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    if (bind(sockfd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) < 0) {
        spdlog::critical("Unable to bind port {}", port);
        exit(EXIT_FAILURE);
    }

    if (listen(sockfd, 1) < 0) {
        spdlog::critical("Unable to listen");
        exit(EXIT_FAILURE);
    }

    if (fcntl(sockfd, F_SETFL, O_NONBLOCK) == -1) {
        spdlog::critical("Unable to set socket to non blocking");
        exit(EXIT_FAILURE);
    }

    return sockfd;
}

bool App::is_ssl_enabled() const { return ssl_ctx != nullptr; }

void App::create_ssl_client(int new_socket) {
    auto new_client_info = std::make_unique<SslClient>(new_socket, router_);
    new_client_info->handshake(ssl_ctx);
    if (!new_client_info->is_active()) {
        // TODO: We probably want to redirect the user to https instead of closing the connection
        new_client_info->close_connection();
        throw std::runtime_error("handshake failed");
    }
    std::unique_lock lock(clients_mutex_);
    clients[new_socket] = std::move(new_client_info);
    lock.unlock();
}

void App::create_socket_client(int new_socket) {
    auto new_client_info = std::make_unique<SocketClient>(new_socket, router_);
    std::unique_lock lock(clients_mutex_);
    clients[new_socket] = std::move(new_client_info);
    lock.unlock();
}

bool App::handle_client(EventLoop &event_loop, const int i) {
    // Retrieve the client
    if (const auto it = clients.find(i); it != clients.end()) {
        it->second->socket_read();

        std::unique_lock lock(clients_mutex_);
        if (!it->second->is_active()) {
            event_loop.remove_fd(i);
            it->second->close_connection();
            clients.erase(it);
            return false;
        }
        lock.unlock();
    } else {
        spdlog::critical("Client {} has disappeared", i);
        event_loop.remove_fd(i);
        return false;
    }

    return true;
}

void App::accept_new(EventLoop &event_loop) {
    sockaddr_in addr{};
    uint len = sizeof(addr);

    const int new_socket = accept(sockfd, reinterpret_cast<struct sockaddr *>(&addr), &len);
    if (new_socket < 0) {
        throw std::runtime_error("unable to accept new connection");
    }

    // Set new socket to non-blocking
    if (fcntl(new_socket, F_SETFL, O_NONBLOCK) == -1) {
        throw std::runtime_error("unable to set the connection to non-blocking");
    }

    if (is_ssl_enabled()) {
        create_ssl_client(new_socket);
    } else {
        create_socket_client(new_socket);
    }

    event_loop.add_fd(new_socket);
}

void App::run(const int port) {
    sockfd = create_socket(port);

    spdlog::info("Server {} listening on port {}", sockfd, port);

    constexpr int max_events = 256;

    auto event_loop = EpollEventLoop();
    event_loop.add_fd(sockfd);

    auto *events = new epoll_event[max_events];

    while (is_running) {
        const int num_events = event_loop.wait_for_events(events, max_events);
        if (num_events < 0) {
            break;
        }

        for (int i = 0; i < num_events; ++i) {
            const auto fd = events[i].data.fd;
            bool is_valid = true;

            if (fd == sockfd) {
                try {
                    accept_new(event_loop);
                } catch (const std::exception &e) {
                    spdlog::warn("Unable to accept client: '{}'", e.what());
                }
            } else {
                is_valid = handle_client(event_loop, fd);
            }

            if (is_valid) {
                event_loop.modify_fd(fd);
            }
        }
    }

    delete[] events;
    close_socket();
}

void App::close_socket() {
    if (sockfd >= 0) {
        shutdown(sockfd, SHUT_RDWR);
        close(sockfd);
        sockfd = -1;
    }
    if (ssl_ctx != nullptr) {
        SSL_CTX_free(ssl_ctx);
        ssl_ctx = nullptr;
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

App &App::with_shutdown(const std::atomic<bool> *atomic) {
    is_running = atomic;
    return *this;
}

App::~App() {
    is_running = false;
}
