#include "Response.h"
#include "../request/Request.h"
#include "../string_manipulation.h"

std::map<int, std::string> Response::codes = {{100, "Continue"},
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
                                              {511, "Network Authentication Required"}};

void Response::set_body(const std::string &new_body) { body_ = new_body; }

void Response::set_header(const std::string &key, const std::string &value) {
    std::string lower_key = key;
    to_lower_case_in_place(lower_key);

    headers.emplace(lower_key, value);
}

std::string Response::build() {
    std::string response = "HTTP/1.1 " + get_status_message() + "\r\n";

    if (!body_.empty()) {
        this->set_header("content-length", std::to_string(body_.size()));
    }

    for (const auto &[key, value] : headers) {
        response.append(key).append(": ").append(value).append("\r\n");
    }
    response.append("\r\n");
    if (!body_.empty()) {
        response.append(body_.c_str(), body_.size());
    }
    return response;
}

std::string Response::get_body() const { return body_; }

std::string Response::get_status_message() {
    if (response_code == 0) {
        response_code = body_.empty() ? 204 : 200;
    }
    return std::to_string(response_code) + " " + codes[response_code];
}

void Response::set_status_code(const int &code) { response_code = code; }
