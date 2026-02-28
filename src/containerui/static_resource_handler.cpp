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
    if (req.method != "GET") {
        result = req.respond_empty(MHD_HTTP_METHOD_NOT_ALLOWED);
        return true;
    }

    result = req.respond_static(MHD_HTTP_OK, _contents, _mimetype);
    return true;
}


}