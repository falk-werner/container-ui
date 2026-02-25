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

bool static_resource_handler::can_handle(std::string const & url)
{
    return (url == _url);
}

MHD_Result static_resource_handler::handle(MHD_Connection * connection, std::string const & url)
{
    auto * response = MHD_create_response_from_buffer(
        _contents.size(),
        const_cast<void*>(reinterpret_cast<void const*>(_contents.c_str())),
        MHD_RESPMEM_PERSISTENT);
    if (response == nullptr) {
        return MHD_NO;
    }
    MHD_add_response_header(response, "Content-Type", _mimetype.c_str());

    auto ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    return ret;
}


}