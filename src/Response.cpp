#include "Response.h"

std::map<int, std::string> Response::codes = {
        {100, "Continue"},
        {101, "Switching Protocol"},
        {103, "Processing"},
        {200, "OK"},
        {201, "Created"},
        {202, "Accepted"},
        {203, "Non-Authoritative Information"},
        {204, "No Content"},
        {205, "Reset Content"},
        {206, "Partial Content"},
        {207, "Multi-Status"},
        {208, "Multi-Status"},
        {226, "IM Used"},
        {300, "Multiple Choice"},
        {301, "Moved Permanently"},
        {302, "Found"},
        {303, "See Other"},
        {304, "Not Modified"},
        {305, "Use Proxy"},
        {306, "unused"},
        {307, "Temporary Redirect"},
        {308, "Permanent Redirect"},
        {400, "Bad Request"},
        {401, "Unauthorized"},
        {402, "Payment Required"},
        {403, "Forbidden"},
        {404, "Not Found"},
        {405, "Method Not Allowed"},
        {406, "Not Acceptable"},
        {407, "Proxy Authentication Required"},
        {408, "Request Timeout"},
        {409, "Conflict"},
        {410, "Gone"},
        {411, "Length Required"},
        {412, "Precondition Failed"},
        {413, "Payload Too Large"},
        {414, "URI Too Long"},
        {415, "Unsupported Media Type"},
        {416, "Requested Range Not Satisfiable"},
        {417, "Expectation Failed"},
        {418, "I'm a teapot"},
        {421, "Misdirected Request"},
        {422, "Unprocessable Entity"},
        {423, "Locked"},
        {424, "Failed Dependency"},
        {426, "Upgrade Required"},
        {428, "Precondition Required"},
        {429, "Too Many Requests"},
        {431, "Request Header Fields Too Large"},
        {451, "Unavailable For Legal Reasons"},
        {500, "Internal Server Error"},
        {501, "Not Implemented"},
        {502, "Bad Gateway"},
        {503, "Service Unavailable"},
        {504, "Gateway Timeout"},
        {505, "HTTP Version Not Supported"},
        {506, "Variant Also Negotiates"},
        {507, "Insufficient Storage"},
        {508, "Loop Detected"},
        {510, "Not Extended"},
        {511, "Network Authentication Required"}
};

Response::Response() {
    http_version = "1.0";
    response_code = 200;
}

void Response::setHeader(const std::string &key, const std::string &value) {
    this->headers.insert({key, value});
}

void Response::setBody(const std::string &b) {
    this->body = b;
}

std::string Response::getHeadersAsString() {
    std::string response;

    response += "HTTP/" + http_version + " " + std::to_string(response_code) + " " + codes.at(response_code) + "\n";

    std::map<std::string, std::string>::iterator it, end;
    for (it = headers.begin(), end = headers.end(); it != end; ++it)
        response += it->first + ": " + it->second + "\n";

    response += "\n";

    return response;
}

std::string Response::toString() {
    std::string response;

    response += "HTTP/" + http_version + " " + std::to_string(response_code) + " " + codes.at(response_code) + "\n";

    std::map<std::string, std::string>::iterator it, end;
    for (it = headers.begin(), end = headers.end(); it != end; ++it)
        response += it->first + ": " + it->second + "\n";

    response += "\n" + body;

    return response;
}

void Response::setResponseCode(const int &code) {
    response_code = code;
}
