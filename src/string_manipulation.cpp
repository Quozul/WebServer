#include "string_manipulation.h"

#include <algorithm>
#include <sstream>


void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch) && ch != '\r' && ch != '\n';
    }));
}

void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch) && ch != '\r' && ch != '\n';
    }).base(), s.end());
}

void trim(std::string &s) {
    rtrim(s);
    ltrim(s);
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

std::unordered_map<std::string, std::string> parse_key_value(std::string &rawHeaders) {
    std::unordered_map<std::string, std::string> headers;
    std::istringstream iss(rawHeaders);

    while (std::getline(iss, rawHeaders, '\n')) {
        const size_t equalIndex = rawHeaders.find(':');
        if (equalIndex == std::string::npos) {
            continue;
        }
        std::string key = rawHeaders.substr(0, equalIndex);
        std::string value = rawHeaders.substr(equalIndex + 1);
        trim(value);
        trim(key);
        to_lower_case_in_place(key);
        headers[key] = value;
    }

    return headers;
}
