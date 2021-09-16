#include "utils.hpp"

// https://stackoverflow.com/a/16749483
std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        if (item.length() > 0) {
            elems.push_back(item);
        }
    }
    return elems;
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

void serveCharArray(SSL *ssl, const char *res, size_t len) {
    char *buf = new char[READ_SIZE];

    for (size_t i = 0; i < len; i += READ_SIZE) {
        size_t end = std::min(i + READ_SIZE, len);
        size_t l = end - i;
        std::memcpy(buf, res + i, l);
        SSL_write(ssl, buf, l);
    }

    delete[] buf;
}

void serveString(SSL *ssl, std::string &str) {
    // Convert to char* then send response
    size_t len = str.length();
    char *buf = new char[READ_SIZE];
    const char *res = str.c_str();

    for (size_t i = 0; i < len; i += READ_SIZE) {
        size_t end = std::min(i + READ_SIZE, len);
        size_t l = end - i;
        std::memcpy(buf, res + i, l);
        SSL_write(ssl, buf, l);
    }

    delete[] buf;
}