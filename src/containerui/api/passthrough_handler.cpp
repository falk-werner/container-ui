#include "containerui/api/api_handlers.hpp"
#include "containerui/util/fetch.hpp"
#include "containerui/util/url_util.hpp"

#include <iostream>
#include <sstream>

namespace container_ui
{

passthrough_handler::passthrough_handler(
    std::string const & path,
    authenticator & auth)
: _url(std::string("/api/") + path)
, _remote_url(std::string("http://localhost/") + path)
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

    std::stringstream url;
    url << _remote_url;
    char sep = '?';
    for(auto const & entry: req.get_all_query_args()) {
        url << sep << url_encode(entry.key) << '=' << url_encode(entry.value);
        sep = '&';
    }

    auto const resp = fetch(url.str(), "/var/run/docker.sock");
    result = req.respond(resp.status, resp.contents, "application/json");
    return true;
}


}