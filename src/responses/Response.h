#ifndef RESPONSEBUILDER_H
#define RESPONSEBUILDER_H

#include "Response.h"
#include <map>
#include <string>

class Response {
    std::string body;
    std::map<std::string, std::string> headers;
    int response_code = 200;
    static std::map<int, std::string> codes;

  public:
    Response() = default;

    void set_body(const std::string &body);

    void set_header(const std::string &key, const std::string &value);

    void set_status_code(const int &code);

    std::string build();

    [[nodiscard]] std::string get_status_message() const;
};

#endif
