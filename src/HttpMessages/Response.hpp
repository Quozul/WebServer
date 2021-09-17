#ifndef WEBSERVER_RESPONSE_HPP
#define WEBSERVER_RESPONSE_HPP

#include <string>
#include <map>
#include <openssl/ssl.h>
#include "../utils.hpp"

class Response {
private:
    std::string http_version;
    unsigned short response_code;
    std::map<std::string, std::string> headers;
    std::string body;
    SSL *ssl;

    static std::map<int, std::string> codes;

public:
    bool headers_sent = false;

    Response();

    void setHeader(const std::string &key, const std::string &value);

    /**
     * Body should be sent manually
     * @param b
     */
    void setBody(const std::string &b);

    void setResponseCode(const int &code);

    std::string getHeadersAsString();

    void sendHeaders();

    /**
     * Should only be used if the response has a body
     * @return The full response, including the body
     */
    std::string toString();

    void setSSL(SSL *s) {
        this->ssl = s;
    }

    SSL *getSSL() {
        return this->ssl;
    }
};


#endif //WEBSERVER_RESPONSE_HPP
