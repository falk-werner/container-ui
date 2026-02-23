#ifndef CONTAINERUI_FETCH_HPP
#define CONTAINERUI_FETCH_HPP

#include <string>

namespace container_ui
{

std::string fetch(
    std::string const & url,
    std::string const & unix_socket);

}

#endif
