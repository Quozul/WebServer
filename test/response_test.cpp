#include "../src/response/Response.h"
#include <gtest/gtest.h>

TEST(ResponseTest, CompleteTest) {
    // Given
    Response response;
    response.set_body("Hello, world!");
    response.set_header("content-type", "text/html");
    response.set_status_code(201);

    // When
    const auto result = response.build();

    // Then
    EXPECT_EQ(result, "HTTP/1.1 201 Created\r\ncontent-length: 13\r\ncontent-type: text/html\r\n\r\nHello, world!");
}

TEST(ResponseTest, EmptyResponseShouldHaveNoContentStatus) {
    // Given
    Response response;

    // When
    const auto result = response.build();

    // Then
    EXPECT_EQ(result, "HTTP/1.1 204 No Content\r\n\r\n");
}

TEST(ResponseTest, ShouldHandleResponseWithOnlyBody) {
    // Given
    Response response;
    response.set_body("Hello, world!");

    // When
    const auto result = response.build();

    // Then
    EXPECT_EQ(result, "HTTP/1.1 200 OK\r\ncontent-length: 13\r\n\r\nHello, world!");
}

TEST(ResponseTest, HeaderShouldBeCaseInsensitive) {
    // Given
    Response response;
    response.set_header("CONTENT-type", "Text/Html");

    // When
    const auto result = response.build();

    // Then
    EXPECT_EQ(result, "HTTP/1.1 204 No Content\r\ncontent-type: Text/Html\r\n\r\n");
}

TEST(ResponseTest, ShouldHandleResponseWithOnlyStatusCode) {
    // Given
    Response response;
    response.set_status_code(400);

    // When
    const auto result = response.build();

    // Then
    EXPECT_EQ(result, "HTTP/1.1 400 Bad Request\r\n\r\n");
}

TEST(ResponseTest, ShouldHandleResponseWithOnlyBodyAndHeaders) {
    // Given
    Response response;
    response.set_body("Hello, world!");
    response.set_header("content-type", "text/html");

    // When
    const auto result = response.build();

    // Then
    EXPECT_EQ(result, "HTTP/1.1 200 OK\r\ncontent-length: 13\r\ncontent-type: "
                      "text/html\r\n\r\nHello, world!");
}
