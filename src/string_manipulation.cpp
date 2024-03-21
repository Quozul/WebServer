#include "string_manipulation.h"

#include <algorithm>
#include <sstream>


std::string &ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch) && ch != '\r' && ch != '\n';
    }));
    return s;
}

std::string &rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch) && ch != '\r' && ch != '\n';
    }).base(), s.end());
    return s;
}

std::string &trim(std::string &s) {
    rtrim(s);
    ltrim(s);
    return s;
}

void to_lower_case_in_place(std::string &string) {
    std::transform(string.begin(), string.end(), string.begin(), [](const unsigned char character) {
        return std::tolower(character);
    });
}

void to_upper_case_in_place(std::string &string) {
    std::transform(string.begin(), string.end(), string.begin(), [](const unsigned char character) {
        return std::toupper(character);
    });
}

std::unordered_map<std::string, std::string> parse_key_value(std::string &raw_headers) {
    std::unordered_map<std::string, std::string> headers;
    std::istringstream iss(raw_headers);

    while (std::getline(iss, raw_headers, '\n')) {
        const size_t equal_index = raw_headers.find(':');
        if (equal_index == std::string::npos) {
            continue;
        }
        std::string key = raw_headers.substr(0, equal_index);
        std::string value = raw_headers.substr(equal_index + 1);
        trim(value);
        trim(key);
        to_lower_case_in_place(key);
        headers[key] = value;
    }

    return headers;
}
