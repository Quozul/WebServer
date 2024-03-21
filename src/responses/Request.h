#ifndef REQUEST_H
#define REQUEST_H

#include <string>
#include <unordered_map>
#include <utility>
#include <optional>

#include "../parsers/Url.h"

struct Request {
    std::string protocol;
    std::string method;
    Url url;
    std::unordered_map<std::string, std::string> headers;
    std::string body;

    std::optional<std::string> get_header(const std::string &key) const;

    std::string get_body() const {
        return body;
    }

    Url get_url() const {
        return url;
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
};

std::tuple<std::string, std::string, std::string> get_sections(const std::string &request);

std::tuple<std::string, std::string, std::string> parse_start_line(const std::string &startLine);

std::tuple<std::string, std::unordered_map<std::string, std::string> > parse_url(const std::string &raw_url);

#endif
