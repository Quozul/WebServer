#ifndef CONNECTION_H
#define CONNECTION_H

#include "../responses/Request.h"
#include <string>

#define BUFFER_SIZE 16'384

class Connection {
  public:
    Connection() = default;

    virtual ~Connection() = default;

    virtual Request socket_read() = 0;

    virtual void write_socket(const std::string &body) = 0;

    virtual void close_socket() = 0;

    virtual bool is_open() = 0;
};

int get_buffer_size(size_t remaining);

#endif
