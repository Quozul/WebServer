#include "SocketConnection.h"

#include "../parsers/RequestParser.h"
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>

#define BUFFER_SIZE 16'384

SocketConnection::SocketConnection(const int client) : Connection() {
    this->client = client;
}

Request SocketConnection::socket_read() {
    RequestParser parser{};
    ssize_t bytes_read = 0, pending = 0;
    const auto buffer = new char[BUFFER_SIZE];

    do {
        const auto remaining = static_cast<int>(parser.remaining_bytes());
        const int buffer_size = std::min(BUFFER_SIZE, remaining);

        bytes_read += recv(this->client, buffer, buffer_size, 0);
        pending =
            recv(this->client, buffer, buffer_size, MSG_PEEK | MSG_DONTWAIT);

        parser.append_content(buffer);
    } while (bytes_read > 0 && pending > 0);

    delete[] buffer;
    return parser.request;
}

void SocketConnection::close_socket() { close(this->client); }

void SocketConnection::write_socket(const std::string &body) {
    write(this->client, body.c_str(), body.size());
}
