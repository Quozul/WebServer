#include "SslConnection.h"

#include <unistd.h>
#include <openssl/err.h>


SslConnection::SslConnection(const int client, SSL_CTX *ctx): SocketConnection(client) {
    this->ssl = SSL_new(ctx);
    SSL_set_fd(this->ssl, this->client);

    if (SSL_accept(this->ssl) <= 0) {
        this->is_ready = false;
    } else {
        this->is_ready = true;
    }
}

std::string SslConnection::socket_read() {
    int bytes_read = 0, pending = 0;
    std::string final_buffer;

    do {
        char buffer[1024];
        bytes_read += SSL_read(this->ssl, buffer, 1024);
        pending = SSL_pending(this->ssl);

        final_buffer.append(buffer);
    } while (bytes_read > 0 && pending > 0);

    return final_buffer;
}

void SslConnection::write_socket(const std::string &body) {
    SSL_write(this->ssl, body.c_str(), body.size());
}

void SslConnection::close_socket() {
    SSL_shutdown(this->ssl);
    SSL_free(this->ssl);
    close(this->client);
}
