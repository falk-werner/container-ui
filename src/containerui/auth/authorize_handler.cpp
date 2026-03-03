#include "containerui/auth/authorize_handler.hpp"
#include "containerui/util/post_data.hpp"

#include <sstream>
#include <iostream>
#include <vector>

namespace container_ui
{

namespace
{

constexpr char const auth_html[] = R"(<!DOCTYPE html>
<html>
<head>
    <title>Login</title>
    <meta charset="UTF-8" />
    <link rel="stylesheet" type="text/css" href="/style.css"/>
</head>
<body>
    <div id="login" class="dialog">
        <h1>Container UI Login</h1>
        <form method="post">
            <label for="connect_token">Connect Token: </label>
            <input name="connect_token" type="password"/>
            <input type="submit" value="Login" />
        </form>
    </div>
</body>
</html>
)";

void replace(std::string & value, std::string const & what, std::string const & to)
{
    auto const pos = value.find(what);
    if (pos == std::string::npos) {
        return;
    }

    value.replace(pos, what.size(), to);
}

MHD_Result handle_get(request & req)
{
    auto const response_type = req.get_query_arg("response_type");
    if (response_type.empty()) {
        std::cerr << "error: authorize: missing response_type" << std::endl;
        return req.respond_empty(MHD_HTTP_BAD_REQUEST);
    }

    auto const client_id = req.get_query_arg("client_id");
    if (client_id.empty()) {
        std::cerr << "error: authorize: missing client_id" << std::endl;
        return req.respond_empty(MHD_HTTP_BAD_REQUEST);
    }

    auto const redirect_uri = req.get_query_arg("redirect_uri");
    if (redirect_uri.empty()) {
        std::cerr << "error: authorize: missing redirect_uri" << std::endl;
        return req.respond_empty(MHD_HTTP_BAD_REQUEST);
    }

    auto const scope = req.get_query_arg("scope");
    if (scope.empty()) {
        std::cerr << "error: authorize: missing scope" << std::endl;
        return req.respond_empty(MHD_HTTP_BAD_REQUEST);
    }

    auto const state = req.get_query_arg("state");
    if (state.empty()) {
        std::cerr << "error: authorize: missing state" << std::endl;
        return req.respond_empty(MHD_HTTP_BAD_REQUEST);
    }

    auto const code_challenge_method = req.get_query_arg("code_challenge_method");
    if (code_challenge_method.empty()) {
        std::cerr << "error: authorize: missing code_challenge_method" << std::endl;
        return req.respond_empty(MHD_HTTP_BAD_REQUEST);
    }

    auto const code_challenge = req.get_query_arg("code_challenge");
    if (scope.empty()) {
        std::cerr << "error: authorize: missing code_challenge" << std::endl;
        return req.respond_empty(MHD_HTTP_BAD_REQUEST);
    }

    return req.respond(MHD_HTTP_OK, auth_html, "text/html");
}

MHD_Result handle_post(request & req, authenticator & auth)
{
    MHD_Result result = MHD_NO;
    std::unordered_map<std::string, std::string> data;
    if (!update_post_data(req, result, data)) {
        return result;
    }

    if (!data.contains("connect_token")) {
        std::cerr << "error: missing connect_token" << std::endl;
        return req.respond_empty(MHD_HTTP_BAD_REQUEST);
    }

    std::string const url = auth.authenticate(
        req.get_query_arg("response_type"),
        req.get_query_arg("client_id"),
        req.get_query_arg("redirect_uri"),
        req.get_query_arg("scope"),
        req.get_query_arg("state"),
        req.get_query_arg("code_challenge_method"),
        req.get_query_arg("code_challenge"),
        data.at("connect_token"));

    if (url.empty()) {
        return req.respond_empty(MHD_HTTP_BAD_REQUEST);
    }

    return req.response_redirect(url);
}

}

authorize_handler::authorize_handler(std::string const & url, authenticator & auth)
: _url(url)
, _auth(auth)
{

}


bool authorize_handler::handle(request & req, MHD_Result & result)
{
    if (req.url != _url) { return false; }

    if (req.method == "GET") {
        result = handle_get(req);
    }
    else if (req.method == "POST") {
        result = handle_post(req, _auth);
    }
    else {
        result = req.respond_empty(MHD_HTTP_METHOD_NOT_ALLOWED);
    }

    return true;
}
   
}