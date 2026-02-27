#include "containerui/util/url_util.hpp"
#include <gtest/gtest.h>

TEST(url_util, escape_nop)
{
    auto const decoded = container_ui::url_decode("foo");
    ASSERT_EQ("foo", decoded);
}

TEST(url_util, escape)
{
    auto const decoded = container_ui::url_decode("some%20value");
    ASSERT_EQ("some value", decoded);
}

TEST(url_util, unescape_nop)
{
    auto const encoded = container_ui::url_encode("foo");
    ASSERT_EQ("foo", encoded);
}

TEST(url_util, unescape)
{
    auto const encoded = container_ui::url_encode("some value");
    ASSERT_EQ("some%20value", encoded);
}
