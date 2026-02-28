#include "containerui/passthrough_handler.hpp"
#include "containerui/util/fetch.hpp"

#include <iostream>

namespace container_ui
{

passthrough_handler::passthrough_handler(
    std::string const url,
    std::string remote_url,
    authenticator & auth)
: _url(url)
, _remote_url(remote_url)
, _auth(auth)
{

}

bool passthrough_handler::handle(
    request & req,
    MHD_Result & result)
{
    if (req.url != _url) { return false; }    
    if (req.method != "GET") { 
        result = req.respond_empty(MHD_HTTP_METHOD_NOT_ALLOWED);
        return true;
    }

    auto const token = req.get_bearer_token();
    if (!_auth.is_token_valid(token)) {
        result = req.respond_empty(MHD_HTTP_FORBIDDEN);
        return true;
    }


    auto const resp = fetch(_remote_url, "/var/run/docker.sock");
    result = req.respond(resp.status, resp.contents, "application/json");
    return true;
}


}