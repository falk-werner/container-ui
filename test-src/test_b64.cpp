#include "containerui/util/b64.hpp"
#include <gtest/gtest.h>

TEST(b64, encode)
{
    auto const encoded = container_ui::b64_encode("fo");
    ASSERT_EQ("Zm8=", encoded);

    auto const decoded = container_ui::b64_decode(encoded);
    ASSERT_EQ("fo", decoded);
}


TEST(b64url, encode)
{
    auto const encoded = container_ui::b64url_encode("fo");
    ASSERT_EQ("Zm8", encoded);

    auto const decoded = container_ui::b64url_decode(encoded);
    ASSERT_EQ("fo", decoded);
}