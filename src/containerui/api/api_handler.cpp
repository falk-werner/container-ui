#include "containerui/api/api_handler.hpp"
#include "containerui/util/post_data.hpp"
#include "containerui/util/fetch.hpp"
#include "containerui/util/url_util.hpp"

#include <sstream>

namespace container_ui
{

namespace
{

std::string get_url(request & req)
{
    std::string const path = req.url.substr(5);

    std::stringstream url;
    url << "http://localhost/" << path ;
    char sep = '?';
    for(auto const & entry: req.get_all_query_args()) {
        url << sep << url_encode(entry.key) << '=' << url_encode(entry.value);
        sep = '&';
    }

    return url.str();
}

MHD_Result handle_get(request & req)
{
    auto const url = get_url(req);
    auto const resp = fetch(url, "/var/run/docker.sock");
    return req.respond(resp.status, resp.contents, "application/json");
}

MHD_Result handle_post(request & req)
{  
    std::string data;
    MHD_Result result = MHD_NO;
    if (!update_post_data(req, result, data)) {
        return result;
    }

    auto const url = get_url(req);
    auto const content_type = req.get_header("Content-Type");

    auto const resp = post(url, "/var/run/docker.sock", data, content_type);
    if (resp.contents.empty()) {
        return req.respond_empty(resp.status);
    }

    return req.respond(resp.status, resp.contents, "text/plain");
}

MHD_Result handle_delete(request & req)
{
    auto const url = get_url(req);
    auto const resp = fetch(url, "/var/run/docker.sock", "DELETE");
    return req.respond_empty(resp.status);
}


}

api_handler::api_handler(std::vector<path_info> const & paths, authenticator & auth)
: matcher("/api/", paths)
, _auth(auth)
{

}

bool api_handler::handle(request & req, MHD_Result & result)
{
    if (!matcher.matches(req)) { return false; }

    auto const token = req.get_bearer_token();
    if (!_auth.is_token_valid(token)) {
        result = req.respond_empty(MHD_HTTP_FORBIDDEN);
        return true;
    }

    if (req.method == "GET") {
        result = handle_get(req);
        return true;
    }

    if (req.method == "POST") {
        result = handle_post(req);
        return true;
    }

    if (req.method == "DELETE") {
        result = handle_delete(req);
        return true;
    }

    return false;
}

}