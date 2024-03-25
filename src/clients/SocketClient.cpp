#include "SocketClient.h"

#include <spdlog/spdlog.h>
#include <sys/socket.h>

unsigned char SocketClient::socket_read() {
    char buffer[BUFFER_SIZE];

    auto buffer_size = get_buffer_size(parser_.remaining_bytes());
    const ssize_t bytes_received = recv(socket_, buffer, buffer_size, 0);

    if (bytes_received <= 0) {
        close(socket_);
        spdlog::trace("Client {} disconnected", socket_);
        return DISCONNECTED;
    }

    parser_.append_content(std::string(buffer, bytes_received));

    if (!parser_.has_more()) {
        Response response_builder;
        router_.handle_request(parser_.request, response_builder);

        const std::string response = response_builder.build();

        socket_write(response.c_str(), response.size());

        const RequestParser new_parser{};
        parser_ = new_parser;
    }

    return STILL_HERE;
}

void SocketClient::socket_write(const char *data, const size_t size) { write(socket_, data, size); }

void SocketClient::close_connection() {
    spdlog::trace("Client {} disconnected", socket_);
    close(socket_);
}