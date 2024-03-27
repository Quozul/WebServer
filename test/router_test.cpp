#include "../src/Router.h"
#include <gtest/gtest.h>

TEST(RouterTest, ShouldCallRoute) {
    // Given
    auto router = Router();
    router.route("/test", []([[maybe_unused]] const Request &request, Response &response) {
        response.set_body("Hello, world!");
    });
    auto request = Request();
    request.url = Url::parse("/test");
    auto response = Response{};

    // When
    router.handle_request(request, response);

    // Then
    EXPECT_STREQ(response.get_status_message().c_str(), "200 OK");
    EXPECT_STREQ(response.get_body().c_str(), "Hello, world!");
}

TEST(RouterTest, ShouldReturnNotFoundWhenRouteDoesNotExists) {
    // Given
    auto router = Router();
    router.route("/test", []([[maybe_unused]] const Request &request, Response &response) {
        response.set_body("Hello, world!");
    });
    auto request = Request();
    request.url = Url::parse("/unknown");
    auto response = Response{};

    // When
    router.handle_request(request, response);

    // Then
    EXPECT_STREQ(response.get_status_message().c_str(), "404 Not Found");
    EXPECT_STREQ(response.get_body().c_str(), "");
}

TEST(RouterTest, ShouldCallNotFoundHandlerWhenRouteDoesNotExists) {
    // Given
    auto router = Router();
    router.route("/test", []([[maybe_unused]] const Request &request, Response &response) {
        response.set_body("Hello, world!");
    });
    router.not_found(
        []([[maybe_unused]] const Request &request, Response &response) { response.set_body("This is not found"); });
    auto request = Request();
    request.url = Url::parse("/unknown");
    auto response = Response{};

    // When
    router.handle_request(request, response);

    // Then
    EXPECT_STREQ(response.get_status_message().c_str(), "404 Not Found");
    EXPECT_STREQ(response.get_body().c_str(), "This is not found");
}

TEST(RouterTest, ShouldNotOverrideNotFoundStatus) {
    // Given
    auto router = Router();
    router.not_found(
        []([[maybe_unused]] const Request &request, Response &response) { response.set_status_code(500); });
    auto request = Request();
    request.url = Url::parse("/unknown");
    auto response = Response{};

    // When
    router.handle_request(request, response);

    // Then
    EXPECT_STREQ(response.get_status_message().c_str(), "500 Internal Server Error");
    EXPECT_STREQ(response.get_body().c_str(), "");
}
