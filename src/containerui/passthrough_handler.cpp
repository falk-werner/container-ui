#include "containerui/passthrough_handler.hpp"
#include "containerui/fetch.hpp"

namespace container_ui
{

passthrough_handler::passthrough_handler(
    std::string const url,
    std::string remote_url)
: _url(url)
, _remote_url(remote_url)
{

}

bool passthrough_handler::can_handle(std::string const & url)
{
    return (url == _url);
}

MHD_Result passthrough_handler::handle(MHD_Connection * connection)
{
    auto const contents = fetch(_remote_url, "/var/run/docker.sock");
    auto * response = MHD_create_response_from_buffer(
        contents.size(),
        const_cast<void*>(reinterpret_cast<void const*>(contents.c_str())),
        MHD_RESPMEM_MUST_COPY);
    if (response == nullptr) {
        return MHD_NO;
    }

    MHD_add_response_header(response, "Content-Type", "application/json");
    auto ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    return ret;
}


}