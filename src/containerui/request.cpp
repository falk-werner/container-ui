#include "containerui/request.hpp"
#include <algorithm>

namespace container_ui
{

namespace
{

MHD_Result respond_generic(
    MHD_Connection * connection,
    unsigned int status_code,
    std::string const & contents,
    std::string const & content_type,
    MHD_ResponseMemoryMode policy)
{
    auto * const response = MHD_create_response_from_buffer(
        contents.size(),
        const_cast<void*>(reinterpret_cast<void const*>(contents.c_str())),
        policy);
    if (nullptr == response) {
        return MHD_NO;
    }

    MHD_add_response_header(response, "Content-Type", content_type.c_str());

    auto const result = MHD_queue_response(connection, status_code, response);
    MHD_destroy_response(response);
    return result;
}

}

namespace
{
MHD_Result get_all_query_args_iter(
    void *cls,
    enum MHD_ValueKind kind,
    const char *key,
    const char *value)
{
    auto * args = reinterpret_cast<std::vector<query_arg>*>(cls);
    args->push_back({key, value});    
    return MHD_YES;
}

}

std::vector<query_arg> request::get_all_query_args()
{
    std::vector<query_arg> args;
    void * context = reinterpret_cast<void*>(&args);

    MHD_get_connection_values(connection, MHD_GET_ARGUMENT_KIND, get_all_query_args_iter, context);
    return args;
}

std::string request::get_query_arg(std::string const & key, std::string const & default_value)
{
   auto const * const value = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, key.c_str());
   return (value != nullptr) ? value : default_value;
}

std::string request::get_header(std::string const & key, std::string const & default_value)
{
   auto const * const value = MHD_lookup_connection_value(connection, MHD_HEADER_KIND, key.c_str());
   return (value != nullptr) ? value : default_value;
}

std::string request::get_bearer_token()
{
    std::string const prefix = "Bearer ";

    auto header = get_header("Authorization");
    if (header.starts_with(prefix)) {
        return header.substr(prefix.size());
    }
    else {
        return "";
    }
}

MHD_Result request::respond_empty(unsigned int status_code)
{
    auto * const response = MHD_create_response_empty(MHD_RF_NONE);
    if (nullptr == response) {
        return MHD_NO;
    }

    auto const result = MHD_queue_response(connection, status_code, response);
    MHD_destroy_response(response);
    return result;
}

MHD_Result request::respond(unsigned int status_code, std::string const & contents, std::string const & content_type)
{
    return respond_generic(connection, status_code, contents, content_type, MHD_RESPMEM_MUST_COPY);
}

MHD_Result request::respond_static(unsigned int status_code, std::string const & contents, std::string const & content_type)
{
    return respond_generic(connection, status_code, contents, content_type, MHD_RESPMEM_PERSISTENT);
}

MHD_Result request::response_redirect(std::string const & location)
{
    auto * const response = MHD_create_response_empty(MHD_RF_NONE);
    if (nullptr == response) {
        return MHD_NO;
    }

    MHD_add_response_header(response, "Location", location.c_str());

    auto const result = MHD_queue_response(connection, MHD_HTTP_SEE_OTHER, response);
    MHD_destroy_response(response);
    return result;

}


}