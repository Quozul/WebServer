#include <gtest/gtest.h>
#include "../../src/parsers/Url.h"

TEST(UrlTest, ShouldParseSimpleUrl) {
    // Given
    const std::string url_string = "/";

    // When
    const auto url = Url::parse(url_string);

    // Then
    EXPECT_STREQ(url.get_path().c_str(), "/");
    EXPECT_STREQ(url.get_full_url().c_str(), "/");
    EXPECT_EQ(url.get_params().size(), 0);
}

TEST(UrlTest, ShouldParseCompleteUrl) {
    // Given
    const std::string url_string = "/submit-form?foo=bar&baz";

    // When
    const auto url = Url::parse(url_string);

    // Then
    EXPECT_STREQ(url.get_path().c_str(), "/submit-form");
    EXPECT_STREQ(url.get_full_url().c_str(), "/submit-form?foo=bar&baz");

    EXPECT_EQ(url.get_params().size(), 2);
    EXPECT_STREQ(url.get_param("foo").value().c_str(), "bar");
    EXPECT_STREQ(url.get_param("baz").value().c_str(), "");
}
