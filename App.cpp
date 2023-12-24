#include "App.h"

#include "connections/SocketConnection.h"


int create_socket(const int port) {
    sockaddr_in addr{};

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    const int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Unable to create socket");
        exit(EXIT_FAILURE);
    }

    if (bind(sock, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) < 0) {
        perror("Unable to bind");
        exit(EXIT_FAILURE);
    }

    if (listen(sock, 1) < 0) {
        perror("Unable to listen");
        exit(EXIT_FAILURE);
    }

    return sock;
}

void App::run(const int port) {
    this->sockfd = create_socket(port);

    std::cout << "Server listening!" << std::endl;

    while (true) {
        sockaddr_in addr{};
        uint len = sizeof(addr);

        const int client = accept(sockfd, reinterpret_cast<struct sockaddr *>(&addr), &len);
        if (client < 0) {
            perror("Unable to accept");
            break;
        }

        // TODO: Handle upgrade to TLS
        SocketConnection connection(client);
        this->accept_connection(connection);
    }
}

void App::accept_connection(Connection &connection) {
    const auto bytes = connection.socket_read();
    connection.write_socket(bytes);
    connection.close_socket();
}

void App::close_socket() const {
    std::cout << "Closing server." << std::flush;
    close(sockfd);
}
