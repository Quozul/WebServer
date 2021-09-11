#ifndef WEBSERVER_SERVLET_H
#define WEBSERVER_SERVLET_H

extern "C" {
#include <openssl/ssl.h>
#include <openssl/err.h>
}

#include <lua.hpp>
#include <cstring>
#include <fstream>
#include <iostream>

#include "Response.h"
#include "Request.h"

#define READ_SIZE 32

void servelet(SSL* ssl, lua_State * L, struct sockaddr_in& addr);

#endif //WEBSERVER_SERVLET_H
