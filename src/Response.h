#ifndef WEBSERVER_RESPONSE_H
#define WEBSERVER_RESPONSE_H

#include <string>
#include <map>

class Response {
private:
    std::string http_version;
    unsigned short response_code;
    std::map<std::string, std::string> headers;
    std::string body;

    static std::map<int, std::string> codes;

public:
    Response();
    void setHeader(const std::string& key, const std::string& value);
    void setBody(const std::string& b);
    void setResponseCode(const int& code);

    std::string getHeadersAsString();
    std::string toString();
};


#endif //WEBSERVER_RESPONSE_H
