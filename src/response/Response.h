#ifndef RESPONSEBUILDER_H
#define RESPONSEBUILDER_H

#include "Response.h"
#include <map>
#include <optional>
#include <string>

class Response {
    std::string body_;
    std::map<std::string, std::string> headers;
    int response_code = 0;
    static std::map<int, std::string> codes;

  public:
    Response() = default;

    void set_body(const std::string &new_body);

    void set_header(const std::string &key, const std::string &value);

    void set_status_code(const int &code);

    std::string build();

    [[nodiscard]] std::string get_body() const;

    [[nodiscard]] std::string get_status_message();
};

#endif
