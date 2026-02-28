#include "containerui/auth/authorize_handler.hpp"
#include "containerui/util/post_data.hpp"

#include <sstream>
#include <iostream>

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
        <input type="hidden" name="challenge" value="${RESPONSE_TYPE}" />
        <input type="hidden" name="challenge" value="${CLIENT_ID}" />
        <input type="hidden" name="challenge" value="${REDIRECT_URI}" />
        <input type="hidden" name="challenge" value="${SCOPE}" />
        <input type="hidden" name="challenge" value="${STATE}" />
        <input type="hidden" name="challenge" value="${CODE_CHALLENGE_METHOD}" />
        <input type="hidden" name="challenge" value="${CODE_CHALLENGE}" />
        <label for="username">Username: </label>
        <input name="username" type="text" />
        <label for="password">Username: </label>
        <input name="password" type="password" />
        <input type="submit" value="Submit" />
    </form>
</body>
</html>
)";

struct post_context
{
    size_t limit;
    size_t count;
    std::stringstream contents;
};

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
    replace(auth_html, "${REDIRECT_URL}", redirect_uri);
    replace(auth_html, "${SCOPE}", scope);
    replace(auth_html, "${SCOPE}", state);
    replace(auth_html, "${CODE_CHALLENGE_METHOD}", code_challenge_method);
    replace(auth_html, "${CODE_CHALLENGE}", code_challenge);

    return req.respond(MHD_HTTP_OK, auth_html, "text/html");
}

MHD_Result handle_post(request & req, authenticator & auth)
{
    if ((*req.connection_cls) == nullptr)
    {
        post_context * const context = new post_context;
        context->count = 0;
        context->limit = 100 * 1024;
        *req.connection_cls = reinterpret_cast<void*>(context);

        return MHD_YES;
    }

    post_context * const context = reinterpret_cast<post_context*>(*req.connection_cls);
    auto const size = *req.upload_data_size;

    if (size > 0) {
        size_t const new_count = context->count + size;
        if (new_count > context->limit) {
            delete context;
            *req.connection_cls = nullptr;
            return MHD_NO;
        }

        context->contents.write(req.upload_data, size);
        context->count = new_count;
        *req.upload_data_size = 0;
        return MHD_YES;
    }

    auto data = parse_post_data(context->contents.str());
    
    delete context;
    *req.connection_cls = nullptr;

    auto * const response = MHD_create_response_empty(MHD_RF_NONE);
    if (response == nullptr) {
        return MHD_NO;
    }

    bool success = true;

    auto username = data.find("username");
    if (username == data.end()) {
        std::cerr << "error: missing username" << std::endl;
        success = false;
    }

    auto password = data.find("password");
    if (password == data.end()) {
        std::cerr << "error: missing password" << std::endl;
        success = false;
    }

    auto challenge = data.find("challenge");
    if (challenge == data.end()) {
        std::cerr << "error: missing challenge" << std::endl;
        success = false;
    }

    std::string const code = auth.authenticate(username->second, password->second, challenge->second);
    if (code.empty()) {
        std::cerr << "error: invalid credentials" << std::endl;
        success = false;
    }

    if (success) {
        auto const url = std::string("/?code=") + code;
        MHD_add_response_header(response, "Location", url.c_str());
    }
    else {
        MHD_add_response_header(response, "Location", "/?error=authentication%20failed");
    }

    auto const result = MHD_queue_response(req.connection, MHD_HTTP_SEE_OTHER, response);
    MHD_destroy_response(response);

    return result;
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