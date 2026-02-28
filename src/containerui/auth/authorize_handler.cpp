#include "containerui/auth/authorize_handler.hpp"
#include "containerui/util/post_data.hpp"

#include <sstream>
#include <iostream>
#include <vector>

namespace container_ui
{

namespace
{

constexpr char const auth_html_template[] = R"(<!DOCTYPE html>
<html>
<head>
    <title>Login</title>
    <meta charset="UTF-8" />
</head>
<body>
    <form method="post">
        <input type="hidden" name="response_type" value="${RESPONSE_TYPE}" />
        <input type="hidden" name="client_id" value="${CLIENT_ID}" />
        <input type="hidden" name="redirect_uri" value="${REDIRECT_URI}" />
        <input type="hidden" name="scope" value="${SCOPE}" />
        <input type="hidden" name="state" value="${STATE}" />
        <input type="hidden" name="code_challenge_method" value="${CODE_CHALLENGE_METHOD}" />
        <input type="hidden" name="code_challenge" value="${CODE_CHALLENGE}" />
        <label for="username">Username: </label>
        <input name="username" type="text" />
        <label for="password">Username: </label>
        <input name="password" type="password" />
        <input type="submit" value="Submit" />
    </form>
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

    std::string auth_html(auth_html_template);
    replace(auth_html, "${RESPONSE_TYPE}", response_type);
    replace(auth_html, "${CLIENT_ID}", client_id);
    replace(auth_html, "${REDIRECT_URI}", redirect_uri);
    replace(auth_html, "${SCOPE}", scope);
    replace(auth_html, "${STATE}", state);
    replace(auth_html, "${CODE_CHALLENGE_METHOD}", code_challenge_method);
    replace(auth_html, "${CODE_CHALLENGE}", code_challenge);

    return req.respond(MHD_HTTP_OK, auth_html, "text/html");
}

MHD_Result handle_post(request & req, authenticator & auth)
{
    MHD_Result result = MHD_NO;
    std::unordered_map<std::string, std::string> data;
    if (!update_post_data(req, result, data)) {
        return result;
    }

    std::vector<std::string> required_keys = {
        "response_type",
        "client_id",
        "redirect_uri",
        "scope",
        "state",
        "code_challenge_method",
        "code_challenge",
        "username",
        "password"
    };

    for(auto const & key: required_keys) {
        if (!data.contains(key)) {
            std::cerr << "error: missing " << key << std::endl;
            return req.respond_empty(MHD_HTTP_BAD_REQUEST);
        }
    }


    std::string const url = auth.authenticate(
        data.at("response_type"),
        data.at("client_id"),
        data.at("redirect_uri"),
        data.at("scope"),
        data.at("state"),
        data.at("code_challenge_method"),
        data.at("code_challenge"),
        data.at("username"),
        data.at("password"));

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