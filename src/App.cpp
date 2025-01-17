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

    if (bind(sockfd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) < 0) {
        spdlog::critical("Unable to bind port {}", addr.sin_port);
        exit(EXIT_FAILURE);
    }

    if (listen(sockfd, 1) < 0) {
        spdlog::critical("Unable to listen");
        exit(EXIT_FAILURE);
    }

    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT | SO_KEEPALIVE, nullptr, 0);

    if (fcntl(sockfd, F_SETFL, O_NONBLOCK) == -1) {
        spdlog::critical("Unable to set socket to non blocking");
        exit(EXIT_FAILURE);
    }

    return sockfd;
}

bool App::is_ssl_enabled() const { return ssl_ctx != nullptr; }

void App::create_ssl_client(EventLoop &event_loop, int new_socket) {
    auto new_client_info = std::make_unique<SslClient>(new_socket, router_);
    new_client_info->handshake(ssl_ctx);
    if (!new_client_info->is_active()) {
        new_client_info->close_connection();
        return; // Handshake failed
    }
    std::unique_lock lock(mutex_);
    clients[new_socket] = std::move(new_client_info);
    lock.unlock();

    event_loop.add_fd(new_socket);
}

void App::create_socket_client(EventLoop &event_loop, int new_socket) {
    auto new_client_info = std::make_unique<SocketClient>(new_socket, router_);
    std::unique_lock lock(mutex_);
    clients[new_socket] = std::move(new_client_info);
    lock.unlock();

    event_loop.add_fd(new_socket);
}

bool App::handle_client(EventLoop &event_loop, const int i) {
    // Retrieve the client
    if (const auto it = clients.find(i); it != clients.end()) {
        it->second->socket_read();

        std::unique_lock lock(mutex_);
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
        spdlog::warn("Unable to accept");
        return;
    }

    // Set new socket to non blocking
    if (fcntl(new_socket, F_SETFL, O_NONBLOCK) == -1) {
        spdlog::warn("Unable to fcntl");
        return;
    }

    if (is_ssl_enabled()) {
        create_ssl_client(event_loop, new_socket);
    } else {
        create_socket_client(event_loop, new_socket);
    }
}

void App::run(const int port) {
    sockfd = create_socket(port);

    spdlog::info("Server {} listening on port {}", sockfd, port);

    const int max_events = 256;

    auto event_loop = EpollEventLoop();
    event_loop.add_fd(sockfd);

    auto *events = new epoll_event[max_events];

    while (is_running) {
        const int num_events = event_loop.wait_for_events(events, max_events);

        for (int i = 0; i < num_events; ++i) {
            const auto fd = events[i].data.fd;
            bool is_valid = true;

            if (fd == sockfd) {
                accept_new(event_loop);
            } else {
                is_valid = handle_client(event_loop, fd);
            }

            if (is_valid) {
                event_loop.modify_fd(fd);
            }
        }
    }

    delete[] events;
}

void App::close_socket() {
    close(sockfd);
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

App::~App() {
    is_running = false;
    close_socket();
}
