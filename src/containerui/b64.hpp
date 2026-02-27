#ifndef CONTAINERUI_B64URL_HPP
#define CONTAINERUI_B64URL_HPP

#include <string>

namespace container_ui
{

std::string b64_encode(std::string const data);

std::string b64_decode(std::string const data);

std::string b64url_encode(std::string const data);

std::string b64url_decode(std::string const data);

}

#endif
