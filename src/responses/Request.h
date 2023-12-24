#ifndef REQUEST_H
#define REQUEST_H

#include <string>
#include <unordered_map>
#include <utility>
#include <algorithm>


struct Request {
private:
    std::string protocol;
    std::string method;
    std::string path;
    std::unordered_map<std::string, std::string> params;
    std::unordered_map<std::string, std::string> headers;
    std::string body;

public:
    std::string get_header(const std::string &key) const {
        std::string keyCopy = key;

        std::transform(keyCopy.begin(), keyCopy.end(), keyCopy.begin(), [](const unsigned char character) {
            return std::tolower(character);
        });
        return headers.at(keyCopy);
    }

    std::string get_param(const std::string &key) const {
        return params.at(key);
    }

    std::string get_path() const {
        return path;
    }

    static Request parse(const std::string &request);
};

std::tuple<std::string, std::string, std::string> get_sections(const std::string &request);

std::tuple<std::string, std::string, std::string> parse_start_line(const std::string &startLine);

std::tuple<std::string, std::unordered_map<std::string, std::string> > parse_url(const std::string &rawUrl);

std::unordered_map<std::string, std::string> parse_headers(std::string &rawHeaders);

#endif
