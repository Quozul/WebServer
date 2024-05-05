#ifndef CLIENT_H
#define CLIENT_H

#define BUFFER_SIZE 16'384

#include <cstddef>

class Client {
  public:
    Client() = default;

    virtual ~Client() = default;

    virtual void socket_read() = 0;

    virtual void socket_write(const char *data, size_t size) = 0;

    virtual void close_connection() = 0;

    virtual bool is_active() = 0;
};

int get_buffer_size(size_t remaining);

#endif
