#ifndef STRING_MANIPULATION_H
#define STRING_MANIPULATION_H

#include <string>
#include <unordered_map>

std::string &ltrim(std::string &s);

std::string &rtrim(std::string &s);

std::string &trim(std::string &s);

void to_lower_case_in_place(std::string &string);

void to_upper_case_in_place(std::string &string);

std::unordered_map<std::string, std::string> parse_key_values(std::string &raw_headers);

#endif
