#include <gtest/gtest.h>
#include "../src/responses/Request.h"

TEST(RequestTest, ShouldHandleHttp09Request) {
    // Given
    const std::string request_string = "GET / HTTP/0.9\r\n\r\n";

    // When
    const Request request = Request::parse(request_string);

    // Then
    EXPECT_STREQ(request.get_method().c_str(), "GET");
    EXPECT_STREQ(request.get_path().c_str(), "/");
    EXPECT_STREQ(request.get_protocol().c_str(), "HTTP/0.9");
}

TEST(RequestTest, ShouldHandleHttp09RequestWithNoProtocol) {
    // Given
    const std::string request_string = "GET /";

    // When
    const Request request = Request::parse(request_string);

    // Then
    EXPECT_STREQ(request.get_method().c_str(), "GET");
    EXPECT_STREQ(request.get_path().c_str(), "/");
    EXPECT_STREQ(request.get_protocol().c_str(), "HTTP/0.9");
}

TEST(RequestTest, ShouldHandleEmptyRequests) {
    // Given
    const std::string request_string;

    // When
    const Request request = Request::parse(request_string);

    // Then
    EXPECT_STREQ(request.get_method().c_str(), "GET");
    EXPECT_STREQ(request.get_path().c_str(), "/");
    EXPECT_STREQ(request.get_protocol().c_str(), "HTTP/0.9");
    EXPECT_EQ(request.get_headers().size(), 0);
    EXPECT_EQ(request.get_params().size(), 0);
    EXPECT_STREQ(request.get_body().c_str(), "");
}

TEST(RequestTest, HandleCompleteRequest) {
    // Given
    const std::string request_string =
            "POST /submit-form?foo=bar&baz HTTP/1.1\r\nHost: example.com\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: 20\r\n\r\nusername=johndoe&password=secret\n";

    // When
    const Request request = Request::parse(request_string);

    // Then
    EXPECT_STREQ(request.get_method().c_str(), "POST");
    EXPECT_STREQ(request.get_path().c_str(), "/submit-form");
    EXPECT_STREQ(request.get_param("foo").value().c_str(), "bar");
    EXPECT_STREQ(request.get_param("baz").value().c_str(), "");
    EXPECT_STREQ(request.get_protocol().c_str(), "HTTP/1.1");

    EXPECT_STREQ(request.get_header("Host").value().c_str(), "example.com");
    EXPECT_STREQ(request.get_header("Content-Type").value().c_str(), "application/x-www-form-urlencoded");
    EXPECT_STREQ(request.get_header("Content-Length").value().c_str(), "20");

    EXPECT_STREQ(request.get_body().c_str(), "username=johndoe&password=secret\n");
}

TEST(RequestTest, ShouldBeCaseInsensitiveForHeaders) {
    // Given
    const std::string request_string =
            "get /submit-form?FOO=bar&BaZ http/1.1\r\nCONTENT-TYPE: APPLICATION/x-www-form-urlencoded\r\ncontent-length: 20";

    // When
    const Request request = Request::parse(request_string);

    for (auto [fst, snd]: request.get_params()) {
        printf("%s %s\n", fst.c_str(), snd.c_str());
    }

    // Then
    EXPECT_STREQ(request.get_method().c_str(), "GET");
    EXPECT_STREQ(request.get_path().c_str(), "/submit-form");
    EXPECT_STREQ(request.get_param("FOO").value().c_str(), "bar");
    EXPECT_EQ(request.get_param("foo").has_value(), false);
    EXPECT_STREQ(request.get_param("BaZ").value().c_str(), "");
    EXPECT_STREQ(request.get_protocol().c_str(), "HTTP/1.1");


    for (auto [fst, snd]: request.get_headers()) {
        printf("'%s' '%s'\n", fst.c_str(), snd.c_str());
    }

    EXPECT_STREQ(request.get_header("content-type").value().c_str(), "APPLICATION/x-www-form-urlencoded");
    EXPECT_STREQ(request.get_header("Content-Length").value().c_str(), "20");

    EXPECT_STREQ(request.get_body().c_str(), "");
}

TEST(RequestTest, ShouldHandleAdditionnalCharactersInHeaders) {
    // Given
    const std::string request_string =
            "GET / HTTP/1.0\r\nuser-agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.12) Gecko/20130109 Firefox/10.0.12\r\n\r\n";

    // When
    const Request request = Request::parse(request_string);

    // Then
    EXPECT_STREQ(request.get_method().c_str(), "GET");
    EXPECT_STREQ(request.get_path().c_str(), "/");
    EXPECT_EQ(request.get_params().size(), 0);
    EXPECT_STREQ(request.get_protocol().c_str(), "HTTP/1.0");

    EXPECT_STREQ(request.get_header("user-agent").value().c_str(), "Mozilla/5.0 (X11; Linux x86_64; rv:10.0.12) Gecko/20130109 Firefox/10.0.12");

    EXPECT_STREQ(request.get_body().c_str(), "");
}