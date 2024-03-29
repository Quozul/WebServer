#ifndef URLPARSER_H
#define URLPARSER_H

#include <optional>
#include <string>
#include <unordered_map>

class Url {
    std::string full_url;
    std::string path;
    std::unordered_map<std::string, std::string> params;

  public:
    std::optional<std::string> get_param(const std::string &key) const {
        if (params.contains(key)) {
            return std::optional{params.at(key)};
        }

        return std::nullopt;
    }

    std::string get_path() const { return path; }

    std::string get_full_url() const { return full_url; }

    std::unordered_map<std::string, std::string> get_params() const { return params; }

    static Url parse(const std::string &raw_url);

    static Url empty();
};

#endif
