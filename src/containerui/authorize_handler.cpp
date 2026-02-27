#include "containerui/authorize_handler.hpp"
#include "containerui/post_data.hpp"

#include <sstream>
#include <iostream>

namespace container_ui
{

namespace
{

constexpr char const auth_html[] = R"(<!DOCTYPE html>
<html>
<head>
    <title>Login</title>
    <meta charset="UTF-8" />
</head>
<body>
    <form method="post">
        <input type="hidden" name="challenge" value="challenge" />
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

MHD_Result handle_get(request & req)
{
    auto * const response = MHD_create_response_from_buffer(
        sizeof(auth_html),
        const_cast<void*>(reinterpret_cast<void const *>(auth_html)),
        MHD_RESPMEM_PERSISTENT);
    if (response == nullptr) {
        return MHD_NO;
    }

    MHD_add_response_header(response, "Content-Type", "text/html");

    MHD_Result const result = MHD_queue_response(req.connection, MHD_HTTP_OK, response);
    MHD_destroy_response(response);
    return result;
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
        return true;
    }
    else if (req.method == "POST") {
        result = handle_post(req, _auth);
        return true;
    }
    else {
        return false;
    }
}
   
}