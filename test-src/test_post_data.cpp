#include "containerui/post_data.hpp"
#include <gtest/gtest.h>

TEST(post_data, parse_single)
{
    auto data = container_ui::parse_post_data("foo=bar");
    ASSERT_EQ(1, data.size());
    ASSERT_EQ("bar", data["foo"]);
}

TEST(post_data, parse_multiple)
{
    auto data = container_ui::parse_post_data("foo=1&bar=2");
    ASSERT_EQ(2, data.size());
    ASSERT_EQ("1", data["foo"]);
    ASSERT_EQ("2", data["bar"]);
}

TEST(post_data, parse_key_only)
{
    auto data = container_ui::parse_post_data("foo&bar");
    ASSERT_EQ(2, data.size());
    ASSERT_EQ("", data["foo"]);
    ASSERT_EQ("", data["bar"]);
}

TEST(post_data, parse_encoded_value)
{
    auto data = container_ui::parse_post_data("foo=%20");
    ASSERT_EQ(1, data.size());
    ASSERT_EQ(" ", data["foo"]);
}

TEST(post_data, parse_encoded_key)
{
    auto data = container_ui::parse_post_data("some%20key=value");
    ASSERT_EQ(1, data.size());
    ASSERT_EQ("value", data["some key"]);
}
