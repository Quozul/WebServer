#ifndef WEBSERVER_UTILS_HPP
#define WEBSERVER_UTILS_HPP

extern "C" {
#include <openssl/ssl.h>
#include <openssl/err.h>
}

#define READ_SIZE 1024

#include <cstring>
#include <string>
#include <sstream>
#include <vector>
#include <iterator>

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);

std::vector<std::string> split(const std::string &s, char delim);

void serveCharArray(SSL *ssl, const char *res, size_t len);

void serveString(SSL *ssl, std::string &str);

// Source: https://gist.github.com/arthurafarias/56fec2cd49a32f374c02d1df2b6c350f
std::string decodeURIComponent(std::string encoded);

std::string encodeURIComponent(std::string decoded);

#endif //WEBSERVER_UTILS_HPP
