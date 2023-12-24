#ifndef SOCKETCONNECTION_H
#define SOCKETCONNECTION_H

#include "Connection.h"

class SocketConnection : public Connection {
protected:
    int client;

public:
    explicit SocketConnection(int client);

    ~SocketConnection() override = default;

    std::string socket_read() override;

    void write_socket(const std::string &body) override;

    void close_socket() override;
};


#endif
