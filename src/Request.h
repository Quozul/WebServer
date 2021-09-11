#ifndef WEBSERVER_REQUEST_H
#define WEBSERVER_REQUEST_H

#include <string>
#include <map>
#include <iostream>

class Request {
private:
    std::string http_version;
    std::string method;
    std::string path;
    std::map<std::string, std::string> headers;
    std::string body;

public:
    Request(const std::string& request);
    const std::string& getHeader(const std::string& key) {
        return headers.at(key);
    };
    const std::map<std::string, std::string>& getHeaders() {
        return headers;
    };
    const std::string& getBody() {
        return body;
    };
    const std::string& getPath() {
        return path;
    };
    const std::string& getMethod() {
        return method;
    };
    const std::string& getVersion() {
        return http_version;
    };
};


#endif //WEBSERVER_REQUEST_H
