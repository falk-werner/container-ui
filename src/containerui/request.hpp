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

    std::string get_query_arg(std::string const & key, std::string const & default_value = "");
    MHD_Result respond_empty(unsigned int status_code);    
    MHD_Result respond(unsigned int status_code, std::string const & contents, std::string const & content_type);
    MHD_Result respond_static(unsigned int status_code, std::string const & contents, std::string const & content_type);
    MHD_Result response_redirect(std::string const & location);

};

}

#endif
