#include "containerui/util/token.hpp"
#include "containerui/util/b64.hpp"

#include <openssl/rand.h>

namespace container_ui
{

std::string generate_token()
{
    constexpr size_t const size = 42;
    char buffer[size];

    int const rc = RAND_bytes(reinterpret_cast<unsigned char*>(buffer), size);
    if (rc != 1) {
        return "";
    }

    return b64url_encode(std::string(buffer, size));

}

}