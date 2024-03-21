#include <sstream>

#include "Url.h"

Url Url::parse(const std::string &raw_url) {
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
    std::string rawParam;

    while (std::getline(iss, rawParam, '&')) {
        const size_t equalIndex = rawParam.find('=');
        std::string key = rawParam.substr(0, equalIndex);
        const std::string value = equalIndex == std::string::npos ? "" : rawParam.substr(equalIndex + 1);
        params[key] = value;
    }

    url.params = params;
    url.path = path;
    url.full_url = raw_url;

    return url;
}
