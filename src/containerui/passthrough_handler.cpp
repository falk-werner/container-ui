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

bool passthrough_handler::handle(
    request & req,
    MHD_Result & result)
{
    if (req.url != _url) { return false; }
    if (req.method != "GET") { return false; }

    auto const resp = fetch(_remote_url, "/var/run/docker.sock");
    auto * response = MHD_create_response_from_buffer(
        resp.contents.size(),
        const_cast<void*>(reinterpret_cast<void const*>(resp.contents.c_str())),
        MHD_RESPMEM_MUST_COPY);
    if (response == nullptr) {
        return MHD_NO;
    }

    MHD_add_response_header(response, "Content-Type", "application/json");
    result = MHD_queue_response(req.connection, resp.status, response);
    MHD_destroy_response(response);
    return true;
}


}