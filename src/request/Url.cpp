#include <sstream>

#include "Url.h"

#include "../string_manipulation.h"

Url Url::parse(const std::string &raw_url) {
    if (std::string url_copy = raw_url; trim(url_copy).empty()) {
        return empty();
    }

    Url url;
    const size_t param_index = raw_url.find('?');

    if (param_index == std::string::npos) {
        url.full_url = url.path = raw_url;
        return url;
    }

    std::string path = raw_url.substr(0, param_index);
    std::unordered_map<std::string, std::string> params;

    const std::string raw_params = raw_url.substr(param_index + 1);
    std::istringstream iss(raw_params);
    std::string raw_param;

    while (std::getline(iss, raw_param, '&')) {
        const size_t equal_index = raw_param.find('=');
        std::string key = raw_param.substr(0, equal_index);
        const std::string value = equal_index == std::string::npos ? "" : raw_param.substr(equal_index + 1);
        params[key] = value;
    }

    url.params = params;
    url.path = path;
    url.full_url = raw_url;

    return url;
}

Url Url::empty() {
    Url url;

    url.path = "/";
    url.full_url = "/";

    return url;
}
