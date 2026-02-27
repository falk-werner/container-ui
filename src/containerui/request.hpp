#ifndef CONTAINERUI_REQUEST_HPP
#define CONTAINERUI_REQUEST_HPP

#include <microhttpd.h>
#include <string>

namespace container_ui
{

struct request
{
    MHD_Connection * connection;
    std::string const url;
    std::string const method;
    char const * upload_data;
    size_t * upload_data_size;
    void * * connection_cls;
};

}

#endif
