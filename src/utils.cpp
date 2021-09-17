#include "utils.hpp"

#include <regex>

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

std::string decodeURIComponent(std::string encoded) {
    std::string decoded = encoded;
    std::smatch sm;
    std::string haystack;

    int dynamicLength = decoded.size() - 2;

    if (decoded.size() < 3) return decoded;

    for (int i = 0; i < dynamicLength; i++) {
        haystack = decoded.substr(i, 3);

        if (std::regex_match(haystack, sm, std::regex("%[0-9A-F]{2}"))) {
            haystack = haystack.replace(0, 1, "0x");
            std::string rc = {(char) std::stoi(haystack, nullptr, 16)};
            decoded = decoded.replace(decoded.begin() + i, decoded.begin() + i + 3, rc);
        }

        dynamicLength = decoded.size() - 2;
    }

    return decoded;
}

/*std::string encodeURIComponent(std::string decoded) {
    std::ostringstream oss;
    std::regex r("[!'\\(\\)*-.0-9A-Za-z_~]");

    for (char &c: decoded) {
        if (std::regex_match(std::string(c), r)) {
            oss << c;
        } else {
            oss << "%" << std::uppercase << std::hex << (0xff & c);
        }
    }
    return oss.str();
}*/
