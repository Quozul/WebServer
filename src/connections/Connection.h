#ifndef CONNECTION_H
#define CONNECTION_H

#include <string>
#include "../responses/Request.h"


class Connection {
public:
    Connection() = default;

    virtual ~Connection() = default;

    virtual Request socket_read() = 0;

    virtual void write_socket(const std::string &body) = 0;

    virtual void close_socket() = 0;
};


#endif
