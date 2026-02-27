#ifndef CONTAINERUI_URL_UITL_HPP
#define CONTAINERUI_URL_UITL_HPP

#include <string>

namespace container_ui
{

std::string url_encode(std::string const & value);

std::string url_decode(std::string const & value);

}

#endif
