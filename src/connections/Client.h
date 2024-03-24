#ifndef CLIENT_H
#define CLIENT_H

#define BUFFER_SIZE 16'384
#define DISCONNECTED 1
#define STILL_HERE 0
#include "../Router.h"
#include "../parsers/RequestParser.h"

#include <spdlog/spdlog.h>
#include <sys/socket.h>

class Client {
    RequestParser parser_;
    const Router &router_;

  public:
    int socket;
    explicit Client(const int client, const Router &router) : router_(router), socket(client) {}

    unsigned char read() {
        char buffer[BUFFER_SIZE];
        const ssize_t bytes_received = recv(socket, buffer, BUFFER_SIZE, 0);
        spdlog::trace("{} bytes received from client {}", bytes_received, socket);

        if (bytes_received <= 0) {
            close(socket);
            spdlog::trace("Client {} disconnected", socket);
            return DISCONNECTED;
        }

        parser_.append_content(std::string(buffer, bytes_received));

        spdlog::debug("Received data from client {}", socket);

        if (!parser_.has_more()) {
            Response response_builder;
            router_.handle_request(parser_.request, response_builder);

            const std::string response = response_builder.build();

            write(socket, response.c_str(), response.size());

            const RequestParser new_parser{};
            parser_ = new_parser;
        }

        return STILL_HERE;
    }
};

#endif
