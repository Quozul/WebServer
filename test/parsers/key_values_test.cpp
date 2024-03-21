#include <gtest/gtest.h>

#include "../../src/string_manipulation.h"

TEST(KeyValuesTest, ShouldParseMultipleKeyValue) {
    // Given
    std::string input = "foo: bar\nbaz:xxx ";

    // When
    const auto result = parse_key_values(input);

    // Then
    EXPECT_STREQ(result.at("foo").c_str(), "bar");
    EXPECT_STREQ(result.at("baz").c_str(), "xxx");
    EXPECT_EQ(result.size(), 2);
}

TEST(KeyValuesTest, ShouldIgnoreCommentedLine) {
    // Given
    std::string input = "foo: bar\n#baz:xxx";

    // When
    const auto result = parse_key_values(input);

    // Then
    EXPECT_STREQ(result.at("foo").c_str(), "bar");
    EXPECT_FALSE(result.contains("baz"));
    EXPECT_EQ(result.size(), 1);
}

TEST(KeyValuesTest, ShouldIgnoreEmptyLines) {
    // Given
    std::string input = "foo: bar\n\n\nbaz:xxx\n";

    // When
    const auto result = parse_key_values(input);

    // Then
    EXPECT_STREQ(result.at("foo").c_str(), "bar");
    EXPECT_STREQ(result.at("baz").c_str(), "xxx");
    EXPECT_EQ(result.size(), 2);
}