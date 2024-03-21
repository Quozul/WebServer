#ifndef REQUEST_H
#define REQUEST_H

#include <string>
#include <unordered_map>
#include <utility>
#include <optional>

struct Request {
    std::string protocol;
    std::string method;
    std::string full_url;
    std::string path;
    std::unordered_map<std::string, std::string> params;
    std::unordered_map<std::string, std::string> headers;
    std::string body;

    std::optional<std::string> get_header(const std::string &key) const;

    std::optional<std::string> get_param(const std::string &key) const;

    std::string get_path() const {
        return path;
    }

    std::string get_body() const {
        return body;
    }

    std::string get_full_url() const {
        return full_url;
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
};

std::tuple<std::string, std::string, std::string> get_sections(const std::string &request);

std::tuple<std::string, std::string, std::string> parse_start_line(const std::string &startLine);

std::tuple<std::string, std::unordered_map<std::string, std::string> > parse_url(const std::string &rawUrl);

#endif
