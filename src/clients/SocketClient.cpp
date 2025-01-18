#include "SocketClient.h"

#include <spdlog/spdlog.h>
#include <sys/socket.h>

void SocketClient::socket_read() {
    char buffer[BUFFER_SIZE];

    const auto buffer_size = get_buffer_size(parser_.remaining_bytes());
    const ssize_t bytes_received = recv(socket_, buffer, buffer_size, 0);

    if (bytes_received > 0) {
        parser_.append_content(std::string(buffer, bytes_received));
    }

    if (bytes_received == 0) {
        is_connected_ = false;
        return;
    }

    if (!parser_.has_more()) {
        Response response_builder;
        router_.handle_request(parser_.request, response_builder);

        const std::string response = response_builder.build();

        socket_write(response.c_str(), response.size());

        const RequestParser new_parser{};
        parser_ = new_parser;
        is_connected_ = parser_.is_keep_alive();
    }

    if (bytes_received < 0) {
        spdlog::error("recv error: ({}) '{}'", errno, strerror(errno));
        is_connected_ = false;
    }
}

void SocketClient::socket_write(const char *data, const size_t size) { write(socket_, data, size); }

void SocketClient::close_connection() {
    close(socket_);
    is_connected_ = false;
}

bool SocketClient::is_active() { return is_connected_; }
