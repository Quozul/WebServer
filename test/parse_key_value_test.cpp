#include <gtest/gtest.h>
#include "../src/string_manipulation.h"

TEST(ParseKeyValue, ShouldNotIncludeTrailingNewlineNorEmptyLines) {
    // Given
    std::string input = "foo: bar\r\n\r\nbaz: qux\r\n\r\n";

    // When
    const std::unordered_map<std::string, std::string> output = parse_key_values(input);

    // Then
    EXPECT_EQ(output.size(), 2);
    EXPECT_STREQ(output.at("foo").c_str(), "bar");
    EXPECT_STREQ(output.at("baz").c_str(), "qux");
}

TEST(ParseKeyValue, ShouldHandleLfLineBreaks) {
    // Given
    std::string input = "foo: bar\nbaz: qux\n\n";

    // When
    const std::unordered_map<std::string, std::string> output = parse_key_values(input);

    // Then
    EXPECT_EQ(output.size(), 2);
    EXPECT_STREQ(output.at("foo").c_str(), "bar");
    EXPECT_STREQ(output.at("baz").c_str(), "qux");
}

TEST(ParseKeyValue, ShouldNotIncludeRowsWithoutColon) {
    // Given
    std::string input = "foo: bar\nbaz\n\n";

    // When
    const std::unordered_map<std::string, std::string> output = parse_key_values(input);

    // Then
    EXPECT_EQ(output.size(), 1);
    EXPECT_STREQ(output.at("foo").c_str(), "bar");
}
