#ifndef WEBSERVER_REQUEST_H
#define WEBSERVER_REQUEST_H

#include <string>
#include <map>
#include <iostream>

#define CRLF "\r\n"
#define CRLF_CRLF "\r\n\r\n"
#define CRLF_LEN 2
#define CRLF_CRLF_LEN 4

class Request {
private:
    std::string http_version;
    std::string method;

    std::string path; // Path to the request
    std::map<std::string, std::string> headers; // Headers of the request
    std::map<std::string, std::string> params; // URL parameters after the "?" character in the path
    std::string raw_params; // Raw string after the "?" character in the path

    std::string body; // Body of the request

public:
    explicit Request(const std::string& request);

    const std::string& getHeader(const std::string& key) {
        return headers.at(key);
    };
    const std::map<std::string, std::string>& getHeaders() {
        return headers;
    };

    const std::string& getParameter(const std::string& key) {
        return params.at(key);
    };
    const std::map<std::string, std::string>& getParameters() {
        return params;
    };

    const std::string& getBody() {
        return body;
    };
    const std::string& getRawParams() {
        return raw_params;
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
