#include "SocketConnection.h"

#include <iostream>
#include <unistd.h>
#include <sys/socket.h>


SocketConnection::SocketConnection(const int client) : Connection() {
    this->client = client;
}

std::string SocketConnection::socket_read() {
    std::cout << "Reading!" << std::endl;
    std::string final_buffer;

    ssize_t bytes_read = 0, pending = 0;
    do {
        char buffer[1024];

        bytes_read += recv(this->client, buffer, sizeof(buffer), 0);
        pending = recv(this->client, buffer, sizeof(buffer), MSG_PEEK | MSG_DONTWAIT);

        final_buffer.append(buffer);
    } while (bytes_read > 0 && pending > 0);

    return final_buffer.substr(0, bytes_read);
}

void SocketConnection::close_socket() {
    close(this->client);
}

void SocketConnection::write_socket(const std::string &body) {
    write(this->client, body.c_str(), body.size());
}
