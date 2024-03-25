#ifndef CLIENT_H
#define CLIENT_H

#define BUFFER_SIZE 16'384
#define DISCONNECTED 1
#define STILL_HERE 0

#include <cstddef>

class Client {
  public:
    Client() = default;

    virtual ~Client() = default;

    virtual unsigned char socket_read() = 0;

    virtual void close_connection() = 0;
};

int get_buffer_size(size_t remaining);

#endif
