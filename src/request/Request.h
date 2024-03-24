#ifndef REQUEST_H
#define REQUEST_H

#include <optional>
#include <string>
#include <unordered_map>

#include "Body.h"
#include "Url.h"

struct Request {
    std::string protocol;
    std::string method;
    Url url;
    std::unordered_map<std::string, std::string> headers;
    Body body;

    std::optional<std::string> get_header(const std::string &key) const;

    const Body &get_body() const { return body; }

    Url get_url() const { return url; }

    std::string get_method() const { return method; }

    std::string get_protocol() const { return protocol; }

    std::unordered_map<std::string, std::string> get_headers() const { return headers; }
};

#endif
