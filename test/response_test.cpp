#include "../src/responses/Response.h"
#include <gtest/gtest.h>

TEST(ResponseTest, CompleteTest) {
    Response response;
    response.set_body("Hello, world!");
    response.set_header("content-type", "text/html");
    response.set_status_code(200);

    EXPECT_EQ(response.build(),
              "HTTP/1.1 200 OK\r\ncontent-length: 13\r\ncontent-type: "
              "text/html\r\n\r\nHello, world!");
}

TEST(ResponseTest, ShouldHandleEmptyResponse) {
    Response response;

    EXPECT_EQ(response.build(), "HTTP/1.1 200 OK\r\ncontent-length: 0\r\n\r\n");
}

TEST(ResponseTest, ShouldHandleResponseWithOnlyBody) {
    Response response;
    response.set_body("Hello, world!");

    EXPECT_EQ(response.build(),
              "HTTP/1.1 200 OK\r\ncontent-length: 13\r\n\r\nHello, world!");
}

TEST(ResponseTest, HeaderShouldBeCaseInsensitive) {
    Response response;
    response.set_header("CONTENT-type", "Text/Html");

    EXPECT_EQ(response.build(), "HTTP/1.1 200 OK\r\ncontent-length: "
                                "0\r\ncontent-type: Text/Html\r\n\r\n");
}

TEST(ResponseTest, ShouldHandleResponseWithOnlyStatusCode) {
    Response response;
    response.set_status_code(400);

    EXPECT_EQ(response.build(),
              "HTTP/1.1 400 Bad Request\r\ncontent-length: 0\r\n\r\n");
}

TEST(ResponseTest, ShouldHandleResponseWithOnlyBodyAndHeaders) {
    Response response;
    response.set_body("Hello, world!");
    response.set_header("content-type", "text/html");

    EXPECT_EQ(response.build(),
              "HTTP/1.1 200 OK\r\ncontent-length: 13\r\ncontent-type: "
              "text/html\r\n\r\nHello, world!");
}