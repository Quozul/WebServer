#include "../../src/string_manipulation.h"
#include <gtest/gtest.h>

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

TEST(ParseKeyValue, ShouldProperlyParseKeyValuesWithSpace) {
    // Given
    std::string input = " foo-bar: baz-qux \na key: with space  \n\n";

    // When
    const std::unordered_map<std::string, std::string> output = parse_key_values(input);

    // Then
    EXPECT_EQ(output.size(), 2);
    EXPECT_STREQ(output.at("foo-bar").c_str(), "baz-qux");
    EXPECT_STREQ(output.at("a key").c_str(), "with space");
}

TEST(ParseKeyValue, ShouldParseMultipleKeyValue) {
    // Given
    std::string input = "foo: bar\nbaz:qux ";

    // When
    const auto result = parse_key_values(input);

    // Then
    EXPECT_STREQ(result.at("foo").c_str(), "bar");
    EXPECT_STREQ(result.at("baz").c_str(), "qux");
    EXPECT_EQ(result.size(), 2);
}

TEST(ParseKeyValue, ShouldIgnoreCommentedLine) {
    // Given
    std::string input = "foo: bar\n#baz: qux";

    // When
    const auto result = parse_key_values(input);

    // Then
    EXPECT_STREQ(result.at("foo").c_str(), "bar");
    EXPECT_FALSE(result.contains("baz"));
    EXPECT_EQ(result.size(), 1);
}

TEST(ParseKeyValue, ShouldIgnoreComments) {
    // Given
    std::string input = "foo: bar\nbaz: qux # this should be ignored";

    // When
    const auto result = parse_key_values(input);

    // Then
    EXPECT_STREQ(result.at("foo").c_str(), "bar");
    EXPECT_STREQ(result.at("baz").c_str(), "qux");
    EXPECT_EQ(result.size(), 2);
}

TEST(ParseKeyValue, ShouldIgnoreFirstCommentedLine) {
    // Given
    std::string input = "#foo: bar\nbaz: qux";

    // When
    const auto result = parse_key_values(input);

    // Then
    EXPECT_STREQ(result.at("baz").c_str(), "qux");
    EXPECT_FALSE(result.contains("foo"));
    EXPECT_EQ(result.size(), 1);
}

TEST(ParseKeyValue, ShouldIgnoreEmptyLines) {
    // Given
    std::string input = "foo: bar\n\n\nbaz:qux\n";

    // When
    const auto result = parse_key_values(input);

    // Then
    EXPECT_STREQ(result.at("foo").c_str(), "bar");
    EXPECT_STREQ(result.at("baz").c_str(), "qux");
    EXPECT_EQ(result.size(), 2);
}