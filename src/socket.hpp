#ifndef WEBSERVER_SOCKET_HPP
#define WEBSERVER_SOCKET_HPP

extern "C" {
// https://stackoverflow.com/a/28031039
#ifdef _WIN32
#ifndef _WIN32_WINNT

#define _WIN32_WINNT 0x0501

#endif

#define NOMINMAX
#pragma comment(lib, "Ws2_32.lib")

#include <winsock2.h>
#include <Ws2tcpip.h>
typedef int uint;

#include <windows.h>

#else

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

typedef int SOCKET;

#endif

#include <openssl/ssl.h>
#include <openssl/err.h>
}

int sockClose(SOCKET sock);

int create_socket(int port);

void init_openssl();

void cleanup_openssl();

SSL_CTX *create_context();

void configure_context(SSL_CTX *ctx, const char *cert, const char *key);

#endif //WEBSERVER_SOCKET_HPP
