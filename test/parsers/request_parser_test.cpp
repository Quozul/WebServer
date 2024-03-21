#include <gtest/gtest.h>
#include "../../src/parsers/RequestParser.h"

TEST(RequestParserTest, ShouldParseGET09HTTPRequest) {
    // Given
    const std::string request_string = "GET / HTTP/0.9\r\n\r\n";
    auto parser = RequestParser{};

    // When
    parser.append_content(request_string);
    const auto request = parser.request;

    // Then
    EXPECT_STREQ(request.method.c_str(), "GET");
    EXPECT_STREQ(request.protocol.c_str(), "HTTP/0.9");
    EXPECT_STREQ(request.url.get_full_url().c_str(), "/");
    EXPECT_TRUE(parser.is_complete());
}

TEST(RequestParserTest, ShouldBeIncompleteWhenEmpty) {
    // Given
    const auto parser = RequestParser{};

    // When

    // Then
    EXPECT_FALSE(parser.is_complete());
    EXPECT_EQ(parser.remaining_bytes(), -1);
}

TEST(RequestParserTest, ShouldParseGET11HTTPRequest) {
    // Given
    const std::string request_string = "GET /hello HTTP/1.1\r\n\r\n";
    auto parser = RequestParser{};

    // When
    parser.append_content(request_string);
    const auto request = parser.request;

    // Then
    EXPECT_STREQ(request.method.c_str(), "GET");
    EXPECT_STREQ(request.protocol.c_str(), "HTTP/1.1");
    EXPECT_STREQ(request.url.get_full_url().c_str(), "/hello");
    EXPECT_TRUE(parser.is_complete());
}

TEST(RequestParserTest, ShouldParseRequestWithHeaders) {
    // Given
    const std::string request_string =
            "GET / HTTP/1.0\r\nuser-agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.12) Gecko/20130109 Firefox/10.0.12";
    auto parser = RequestParser{};

    // When
    parser.append_content(request_string);
    const auto request = parser.request;

    // Then
    EXPECT_STREQ(request.method.c_str(), "GET");
    EXPECT_STREQ(request.protocol.c_str(), "HTTP/1.0");
    EXPECT_STREQ(request.get_header("user-agent").value().c_str(),
                 "Mozilla/5.0 (X11; Linux x86_64; rv:10.0.12) Gecko/20130109 Firefox/10.0.12");
    EXPECT_TRUE(parser.is_complete());
}

TEST(RequestParserTest, ShouldParseRequestWithHeadersWithBodyStart) {
    // Given
    const std::string request_string =
            "GET / HTTP/1.0\r\nuser-agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.12) Gecko/20130109 Firefox/10.0.12\r\n\r\n";
    auto parser = RequestParser{};

    // When
    parser.append_content(request_string);
    const auto request = parser.request;

    // Then
    EXPECT_STREQ(request.method.c_str(), "GET");
    EXPECT_STREQ(request.protocol.c_str(), "HTTP/1.0");
    EXPECT_STREQ(request.get_header("user-agent").value().c_str(),
                 "Mozilla/5.0 (X11; Linux x86_64; rv:10.0.12) Gecko/20130109 Firefox/10.0.12");
    EXPECT_TRUE(parser.is_complete());
}

TEST(RequestParserTest, HandleCompleteRequest) {
    // Given
    const std::string request_string =
            "POST /submit-form?foo=bar&baz HTTP/1.1\r\nHost: example.com\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: 33\r\n\r\nusername=johndoe&password=secret\n";
    auto parser = RequestParser{};

    // When
    parser.append_content(request_string);
    const auto request = parser.request;

    // Then
    EXPECT_STREQ(request.method.c_str(), "POST");
    EXPECT_STREQ(request.protocol.c_str(), "HTTP/1.1");
    EXPECT_STREQ(request.url.get_full_url().c_str(), "/submit-form?foo=bar&baz");

    EXPECT_STREQ(request.get_header("Host").value().c_str(), "example.com");
    EXPECT_STREQ(request.get_header("Content-Type").value().c_str(), "application/x-www-form-urlencoded");
    EXPECT_STREQ(request.get_header("Content-Length").value().c_str(), "33");

    EXPECT_STREQ(request.body.c_str(), "username=johndoe&password=secret\n");
    EXPECT_EQ(parser.remaining_bytes(), 0);
    EXPECT_TRUE(parser.is_complete());
}

TEST(RequestParserTest, HandleIncompleteRequest) {
    // Given
    const std::string request_string = "POST /form HTTP/1.1\r\nContent-Length: 33\r\n\r\nusername=johndoe&pas";
    auto parser = RequestParser{};

    // When
    parser.append_content(request_string);
    const auto request = parser.request;

    // Then
    EXPECT_STREQ(request.method.c_str(), "POST");
    EXPECT_STREQ(request.protocol.c_str(), "HTTP/1.1");
    EXPECT_STREQ(request.url.get_full_url().c_str(), "/form");

    EXPECT_STREQ(request.get_header("Content-Length").value().c_str(), "33");

    EXPECT_STREQ(request.body.c_str(), "username=johndoe&pas");
    EXPECT_EQ(parser.remaining_bytes(), 13);
    EXPECT_FALSE(parser.is_complete());
}

TEST(RequestParserTest, HandleCompleteIncompleteRequest) {
    // Given
    const std::string first_part = "POST /form HTTP/1.1\r\nContent-Length: 33\r\n\r\nusername=johndoe&pas";
    const std::string second_part = "sword=secret\n";
    auto parser = RequestParser{};

    // When
    parser.append_content(first_part);
    parser.append_content(second_part);
    const auto request = parser.request;

    // Then
    EXPECT_STREQ(request.method.c_str(), "POST");
    EXPECT_STREQ(request.protocol.c_str(), "HTTP/1.1");

    EXPECT_STREQ(request.get_header("Content-Length").value().c_str(), "33");

    EXPECT_STREQ(request.body.c_str(), "username=johndoe&password=secret\n");
    EXPECT_EQ(parser.remaining_bytes(), 0);
    EXPECT_TRUE(parser.is_complete());
}
