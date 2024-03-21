#ifndef SOCKETCONNECTION_H
#define SOCKETCONNECTION_H

#include "Connection.h"
#include "../parsers/RequestParser.h"

class SocketConnection : public Connection {
protected:
    int client;

public:
    explicit SocketConnection(int client);

    ~SocketConnection() override = default;

    Request socket_read() override;

    void write_socket(const std::string &body) override;

    void close_socket() override;
};


#endif
