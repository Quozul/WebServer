#ifndef REQUEST_H
#define REQUEST_H

#include <string>
#include <unordered_map>
#include <utility>
#include <optional>

#include "../string_manipulation.h"

struct Request {
private:
    std::string protocol;
    std::string method;
    std::string path;
    std::unordered_map<std::string, std::string> params;
    std::unordered_map<std::string, std::string> headers;
    std::string body;

public:
    std::optional<std::string> get_header(const std::string &key) const {
        std::string keyCopy = key;
        to_lower_case_in_place(keyCopy);

        if (headers.contains(keyCopy)) {
            return std::optional{headers.at(keyCopy)};
        }

        return std::nullopt;
    }

    std::optional<std::string> get_param(const std::string &key) const {
        if (params.contains(key)) {
            return std::optional{params.at(key)};
        }

        return std::nullopt;
    }

    std::string get_path() const {
        return path;
    }

    std::string get_body() const {
        return body;
    }

    std::string get_method() const {
        return method;
    }

    std::string get_protocol() const {
        return protocol;
    }

    std::unordered_map<std::string, std::string> get_headers() const {
        return headers;
    }

    std::unordered_map<std::string, std::string> get_params() const {
        return params;
    }

    static Request parse(const std::string &request);
};

std::tuple<std::string, std::string, std::string> get_sections(const std::string &request);

std::tuple<std::string, std::string, std::string> parse_start_line(const std::string &startLine);

std::tuple<std::string, std::unordered_map<std::string, std::string> > parse_url(const std::string &rawUrl);

std::unordered_map<std::string, std::string> parse_key_value(std::string &rawHeaders);

#endif
