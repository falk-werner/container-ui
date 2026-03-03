#include "containerui/auth/token_handler.hpp"
#include "containerui/util/post_data.hpp"

#include <vector>
#include <iostream>

namespace container_ui
{

token_handler::token_handler(std::string const & url, authenticator & auth)
: _url(url)
, _auth(auth)
{

}

bool token_handler::handle(request & req, MHD_Result & result)
{
    if (req.url != _url) { return false; }
    if (req.method != "POST") {
        result = req.respond_empty(MHD_HTTP_METHOD_NOT_ALLOWED);
        return true;
    }

    std::string raw_data;
    if (!update_post_data(req, result, raw_data)) {
        return true;
    }
    std::unordered_map<std::string, std::string> data;
    parse_post_data(raw_data, data);

    std::vector<std::string> required_keys = {
        "grant_type",
        "code",
        "code_verifier",
        "client_id",
        "redirect_uri",
    };

    for(auto const & key: required_keys) {
        if (!data.contains(key)) {
            std::cerr << "error: missing " << key << std::endl;
            result = req.respond_empty(MHD_HTTP_BAD_REQUEST);
            return true;
        }
    }

    auto const token = _auth.get_token(
        data.at("grant_type"),
        data.at("code"),
        data.at("code_verifier"),
        data.at("client_id"),
        data.at("redirect_uri"));

    result = req.respond(MHD_HTTP_OK, token, "application/json");
    return true;
}

}