#include "containerui/util/post_data.hpp"
#include <gtest/gtest.h>

TEST(post_data, parse_single)
{
    std::unordered_map<std::string,std::string> data;
    container_ui::parse_post_data("foo=bar", data);
    ASSERT_EQ(1, data.size());
    ASSERT_EQ("bar", data["foo"]);
}

TEST(post_data, parse_multiple)
{
    std::unordered_map<std::string,std::string> data;
    container_ui::parse_post_data("foo=1&bar=2", data);
    ASSERT_EQ(2, data.size());
    ASSERT_EQ("1", data["foo"]);
    ASSERT_EQ("2", data["bar"]);
}

TEST(post_data, parse_key_only)
{
    std::unordered_map<std::string,std::string> data;
    container_ui::parse_post_data("foo&bar", data);
    ASSERT_EQ(2, data.size());
    ASSERT_EQ("", data["foo"]);
    ASSERT_EQ("", data["bar"]);
}

TEST(post_data, parse_encoded_value)
{
    std::unordered_map<std::string,std::string> data;
    container_ui::parse_post_data("foo=%20", data);
    ASSERT_EQ(1, data.size());
    ASSERT_EQ(" ", data["foo"]);
}

TEST(post_data, parse_encoded_key)
{
    std::unordered_map<std::string,std::string> data;
    container_ui::parse_post_data("some%20key=value", data);
    ASSERT_EQ(1, data.size());
    ASSERT_EQ("value", data["some key"]);
}
