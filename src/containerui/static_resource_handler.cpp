#include "containerui/static_resource_handler.hpp"

namespace container_ui
{

static_resource_handler::static_resource_handler(
    std::string const & url,
    std::string const & contents,
    std::string const & mimetype)
: _url(url)
, _contents(contents)
, _mimetype(mimetype)
{

}

bool static_resource_handler::handle(
    request & req,
    MHD_Result & result)
{
    if (req.url != _url) { return false; }
    if (req.method != "GET") { return false; }

    auto * response = MHD_create_response_from_buffer(
        _contents.size(),
        const_cast<void*>(reinterpret_cast<void const*>(_contents.c_str())),
        MHD_RESPMEM_PERSISTENT);
    if (response == nullptr) {
        result = MHD_NO;
        return true;
    }
    MHD_add_response_header(response, "Content-Type", _mimetype.c_str());

    result = MHD_queue_response(req.connection, MHD_HTTP_OK, response);
    MHD_destroy_response(response);
    return true;
}


}