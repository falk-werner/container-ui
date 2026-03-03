#ifndef CONTAINERUI_REQUEST_HPP
#define CONTAINERUI_REQUEST_HPP

#include <microhttpd.h>
#include <string>
#include <vector>

namespace container_ui
{

struct query_arg
{
    std::string key;
    std::string value;
};

struct request
{
    MHD_Connection * connection;
    std::string const url;
    std::string const method;
    char const * upload_data;
    size_t * upload_data_size;
    void * * connection_cls;

    std::vector<query_arg> get_all_query_args();
    std::string get_query_arg(std::string const & key, std::string const & default_value = "");
    std::string get_header(std::string const & key, std::string const & default_value = "");
    std::string get_bearer_token();
    MHD_Result respond_empty(unsigned int status_code);    
    MHD_Result respond(unsigned int status_code, std::string const & contents, std::string const & content_type);
    MHD_Result respond_static(unsigned int status_code, std::string const & contents, std::string const & content_type);
    MHD_Result response_redirect(std::string const & location);

};

}

#endif
