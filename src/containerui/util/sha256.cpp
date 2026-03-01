#include "containerui/util/sha256.hpp"
#include <openssl/sha.h>

namespace container_ui
{

std::string sha256(std::string const & value)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    (void) SHA256(reinterpret_cast<unsigned char const*>(value.c_str()), value.size(), hash);

    return std::string(reinterpret_cast<char*>(hash), SHA256_DIGEST_LENGTH);
}


}