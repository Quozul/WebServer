#include "SocketConnection.h"

#include "../parsers/RequestParser.h"
#include <iostream>
#include <spdlog/spdlog.h>
#include <sys/socket.h>
#include <unistd.h>

SocketConnection::SocketConnection(const int new_client) : Connection() {
    client = new_client;
    client_open = true;
}

Request SocketConnection::socket_read() {
    RequestParser parser{};
    const std::unique_ptr<char[]> buffer(new char[BUFFER_SIZE]);

    while (parser.has_more()) {
        const auto buffer_size = get_buffer_size(parser.remaining_bytes());

        const auto operation = recv(client, buffer.get(), buffer_size, 0);
        if (operation > 0) {
            parser.append_content(std::string(buffer.get(), operation));
        } else {
            spdlog::error("Unknown error");
            throw std::runtime_error("unknown error");
        }
    }

    return parser.request;
}

void SocketConnection::close_socket() {
    close(client);
    client_open = false;
}

bool SocketConnection::is_open() { return client_open; }

void SocketConnection::write_socket(const std::string &body) { write(client, body.c_str(), body.size()); }
