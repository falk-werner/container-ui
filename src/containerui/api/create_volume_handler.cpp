#include "containerui/api/api_handlers.hpp"
#include "containerui/util/post_data.hpp"
#include "containerui/util/fetch.hpp"

namespace container_ui
{

create_volume_handler::create_volume_handler(authenticator & auth)
: _auth(auth)
{

}

bool create_volume_handler::handle(
    request & req,
    MHD_Result & result)
{
    if (req.url != "/api/volumes/create") { return false; }
    if (req.method != "POST") { return false; }

    std::string data;
    if (!update_post_data(req, result, data)) {
        return true;
    }

    auto const resp = post("http://localhost/volumes/create", "/var/run/docker.sock", data, "application/json");
    result = req.respond_empty(resp.status);

    return true;
}

}