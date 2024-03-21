#include "SocketConnection.h"

#include "../parsers/RequestParser.h"
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>

SocketConnection::SocketConnection(const int client) : Connection() { this->client = client; }

Request SocketConnection::socket_read() {
    RequestParser parser{};
    const auto buffer = new char[BUFFER_SIZE];

    while (!parser.has_more()) {
        const auto buffer_size = get_buffer_size(parser.remaining_bytes());

        if (const auto operation = recv(this->client, buffer, buffer_size, 0); operation > 0) {
            parser.append_content(std::string(buffer, operation));
        }

        parser.append_content(buffer);
    }

    delete[] buffer;
    return parser.request;
}

void SocketConnection::close_socket() { close(this->client); }

void SocketConnection::write_socket(const std::string &body) { write(this->client, body.c_str(), body.size()); }
