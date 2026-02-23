#ifndef CONTAINERUI_WEBSERVER_HPP
#define CONTAINERUI_WEBSERVER_HPP

#include "containerui/webserver_context.hpp"

#include <microhttpd.h>
#include <cstdint>

namespace container_ui
{

class webserver
{
    webserver(webserver const &) = delete;
    webserver& operator=(webserver const &) = delete; 
    webserver(webserver &&) = delete;
    webserver& operator=(webserver &&) = delete; 
public:
    webserver(uint16_t port, webserver_context && context);
    ~webserver();
private:
    webserver_context _context;
    MHD_Daemon * daemon;
};

}

#endif
