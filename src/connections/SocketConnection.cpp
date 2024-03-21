#include "SocketConnection.h"

#include <iostream>
#include <unistd.h>
#include <sys/socket.h>


#define BUFFER_SIZE 16'384

SocketConnection::SocketConnection(const int client) : Connection() {
    this->client = client;
}

Request SocketConnection::socket_read() {
    RequestParser parser{};
    ssize_t bytes_read = 0, pending = 0;

    do {
        const auto remaining = static_cast<int>(parser.remaining_bytes());
        const int buffer_size = std::min(BUFFER_SIZE, remaining);
        const auto buffer = new char[buffer_size];

        bytes_read += recv(this->client, buffer, sizeof(buffer), 0);
        pending = recv(this->client, buffer, sizeof(buffer), MSG_PEEK | MSG_DONTWAIT);

        parser.append_content(buffer);
        delete[] buffer;
    } while (bytes_read > 0 && pending > 0);

    return parser.request;
}

void SocketConnection::close_socket() {
    close(this->client);
}

void SocketConnection::write_socket(const std::string &body) {
    write(this->client, body.c_str(), body.size());
}
