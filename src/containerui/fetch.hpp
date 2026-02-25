#ifndef CONTAINERUI_FETCH_HPP
#define CONTAINERUI_FETCH_HPP

#include <string>

namespace container_ui
{

struct http_response
{
    int status;
    std::string contents;
};

http_response fetch(
    std::string const & url,
    std::string const & unix_socket);

}

#endif
