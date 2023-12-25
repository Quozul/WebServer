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
    EXPECT_STREQ(request.get_full_url().c_str(), "/");
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
    EXPECT_STREQ(request.get_full_url().c_str(), "/");
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
    EXPECT_STREQ(request.get_full_url().c_str(), "/");
    EXPECT_STREQ(request.get_protocol().c_str(), "HTTP/0.9");
    EXPECT_EQ(request.get_headers().size(), 0);
    EXPECT_EQ(request.get_params().size(), 0);
    EXPECT_STREQ(request.get_body().c_str(), "");
}

TEST(RequestTest, JunkShouldFallbackToDefault) {
    // Given
    const std::string request_string = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";

    // When
    const Request request = Request::parse(request_string);

    // Then
    EXPECT_STREQ(request.get_method().c_str(), "GET");
    EXPECT_STREQ(request.get_path().c_str(), "/");
    EXPECT_STREQ(request.get_full_url().c_str(), "/");
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
    EXPECT_STREQ(request.get_full_url().c_str(), "/submit-form?foo=bar&baz");
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

    // Then
    EXPECT_STREQ(request.get_method().c_str(), "GET");
    EXPECT_STREQ(request.get_path().c_str(), "/submit-form");
    EXPECT_STREQ(request.get_full_url().c_str(), "/submit-form?FOO=bar&BaZ");
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
    EXPECT_STREQ(request.get_full_url().c_str(), "/");
    EXPECT_EQ(request.get_params().size(), 0);
    EXPECT_STREQ(request.get_protocol().c_str(), "HTTP/1.0");

    EXPECT_STREQ(request.get_header("user-agent").value().c_str(),
                 "Mozilla/5.0 (X11; Linux x86_64; rv:10.0.12) Gecko/20130109 Firefox/10.0.12");

    EXPECT_STREQ(request.get_body().c_str(), "");
}

TEST(RequestTest, ActualFileUpload) {
    // Given
    const std::string request_string = "POST /file HTTP/1.1\r\n\
Host: localhost:8080\r\n\
User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:121.0) Gecko/20100101 Firefox/121.0\r\n\
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8\r\n\
Accept-Language: en-US,en;q=0.5\r\n\
Accept-Encoding: gzip, deflate, br\r\n\
Referer: https://localhost:8080/file\r\n\
Content-Type: application/x-www-form-urlencoded\r\n\
Content-Length: 26\r\n\
Origin: https://localhost:8080\r\n\
DNT: 1\r\n\
Sec-GPC: 1\r\n\
Connection: keep-alive\r\n\
Upgrade-Insecure-Requests: 1\r\n\
Sec-Fetch-Dest: document\r\n\
Sec-Fetch-Mode: navigate\r\n\
Sec-Fetch-Site: same-origin\r\n\
Pragma: no-cache\r\n\
Cache-Control: no-cache\r\n\
\r\n\
file=large-response%281%29";

    // When
    const Request request = Request::parse(request_string);

    // Then
    EXPECT_STREQ(request.get_method().c_str(), "POST");
    EXPECT_STREQ(request.get_path().c_str(), "/file");
    EXPECT_STREQ(request.get_full_url().c_str(), "/file");
    EXPECT_EQ(request.get_params().size(), 0);
    EXPECT_STREQ(request.get_protocol().c_str(), "HTTP/1.1");
    EXPECT_STREQ(request.get_header("Host").value().c_str(), "localhost:8080");
    EXPECT_STREQ(request.get_header("Cache-Control").value().c_str(), "no-cache");
    EXPECT_STREQ(request.get_header("Accept").value().c_str(), "text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8");
    EXPECT_EQ(request.get_body().size(), 26);
}
