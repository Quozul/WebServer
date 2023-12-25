#include "Request.h"

#include <set>
#include <sstream>

#define CRLF "\r\n"
#define CRLF_CRLF "\r\n\r\n"
#define CRLF_LEN 2
#define CRLF_CRLF_LEN 4

std::set<std::string> methods = {
    "CONNECT",
    "DELETE",
    "GET",
    "HEAD",
    "OPTIONS",
    "PATCH",
    "POST",
    "PUT",
    "TRACE"
};

std::set<std::string> protocols = {
    "HTTP/0.9",
    "HTTP/1.0",
    "HTTP/1.1"
};

Request Request::parse(const std::string &request) {
    auto [startLine, rawHeaders, body] = get_sections(request);
    auto [protocol, method, rawUrl] = parse_start_line(startLine);
    auto [path, params] = parse_url(rawUrl);
    const auto headers = parse_key_value(rawHeaders);

    Request parsedRequest;

    parsedRequest.protocol = protocol;
    parsedRequest.method = method;
    parsedRequest.path = path;
    parsedRequest.params = params;
    parsedRequest.headers = headers;
    parsedRequest.body = body;
    parsedRequest.full_url = rawUrl;

    return parsedRequest;
}

bool is_valid_request_method(const std::string &method) {
    return methods.contains(method);
}

bool is_valid_protocol(const std::string &protocol) {
    return protocols.contains(protocol);
}

std::tuple<std::string, std::string, std::string> get_sections(const std::string &request) {
    const size_t endOfStartLine = request.find(CRLF);

    if (endOfStartLine == std::string::npos) {
        return {request, "", ""};
    }

    const size_t startOfBody = request.find(CRLF_CRLF, endOfStartLine);
    std::string startLine = request.substr(0, endOfStartLine);

    if (startOfBody == endOfStartLine) {
        return {startLine, "", ""};
    }

    std::string headers = request.substr(endOfStartLine + CRLF_LEN, startOfBody - endOfStartLine);

    if (startOfBody == std::string::npos) {
        return {startLine, headers, ""};
    }

    std::string parsedBody = request.substr(startOfBody + CRLF_CRLF_LEN);

    return {startLine, headers, parsedBody};
}

std::tuple<std::string, std::string, std::string> parse_start_line(const std::string &startLine) {
    std::istringstream iss(startLine);
    std::string method, rawUrl, protocol;
    iss >> method >> rawUrl >> protocol;

    to_upper_case_in_place(method);
    to_upper_case_in_place(protocol);

    if (rawUrl.empty()) {
        rawUrl = "/";
    }

    if (protocol.empty() || !is_valid_protocol(protocol)) {
        protocol = "HTTP/0.9";
    }

    if (method.empty() || !is_valid_request_method(method)) {
        method = "GET";
    }

    return {protocol, method, rawUrl};
}

std::tuple<std::string, std::unordered_map<std::string, std::string> > parse_url(const std::string &rawUrl) {
    const size_t paramIndex = rawUrl.find('?');

    if (paramIndex == std::string::npos) {
        return {rawUrl, {}};
    }

    std::string path = rawUrl.substr(0, paramIndex);
    std::unordered_map<std::string, std::string> params;

    const std::string rawParams = rawUrl.substr(paramIndex + 1);
    std::istringstream iss(rawParams);
    std::string rawParam;

    while (std::getline(iss, rawParam, '&')) {
        const size_t equalIndex = rawParam.find('=');
        std::string key = rawParam.substr(0, equalIndex);
        const std::string value = equalIndex == std::string::npos ? "" : rawParam.substr(equalIndex + 1);
        params[key] = value;
    }

    return {path, params};
}
