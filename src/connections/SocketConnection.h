#ifndef SOCKETCONNECTION_H
#define SOCKETCONNECTION_H

#include "Connection.h"

class SocketConnection : public Connection {
  protected:
    int client;
    bool client_open;

  public:
    explicit SocketConnection(int new_client);

    ~SocketConnection() override = default;

    Request socket_read() override;

    void write_socket(const std::string &body) override;

    void close_socket() override;

    bool is_open() override;
};

#endif
