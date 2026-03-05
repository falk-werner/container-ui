#include "containerui/webserver.hpp"
#include "containerui/webserver_context.hpp"
#include "containerui/webcontent.hpp"
#include "containerui/auth/authorize_handler.hpp"
#include "containerui/auth/token_handler.hpp"
#include "containerui/auth/authenticator.hpp"
#include "containerui/api/api_handler.hpp"

#include <unistd.h>

#include <csignal>
#include <iostream>
#include <memory>

using container_ui::webserver;
using container_ui::webserver_context;
using container_ui::authorize_handler;
using container_ui::token_handler;
using container_ui::authenticator;
using container_ui::api_handler;

namespace
{

bool shutdown_requested = false;

void on_shutdown_requested(int signr)
{
    (void) signr;
    shutdown_requested = true;

}

void add_webcontent(webserver_context & context)
{
    context.add_static("/", index_html, index_html_len, "text/html");
    context.add_static("/style.css", style_css, style_css_len, "text/css");
    context.add_static("/script/main.js", script_main_js, script_main_js_len, "text/javascript");
    context.add_static("/script/api.js", script_api_js, script_api_js_len, "text/javascript");
    context.add_static("/script/login.js", script_login_js, script_login_js_len, "text/javascript");
    context.add_static("/script/util.js", script_util_js, script_util_js_len, "text/javascript");
}

void add_oauth_handlers(webserver_context& context, authenticator& auth)
{
    context.add(std::make_unique<authorize_handler>("/auth/authorize", auth));
    context.add(std::make_unique<token_handler>("/auth/token", auth));
}

void add_api_handlers(webserver_context& context, authenticator& auth)
{
    std::vector<std::string> paths = {
        // system
        "version",
        "info",
        "system/df",

        // containers
        "containers/json",
        "containers/{name}/logs",
        "containers/{name}/json",
        "containers/{name}/top",
        "containers/{name}/stats",
        "containers/{name}/start",
        "containers/{name}/stop",
        "containers/{name}/pause",
        "containers/{name}/unpause",
        "containers/{name}/restart",
        "containers/{name}/kill",
        "containers/{name}",

        // images
        "images/json",
        "images/{name}/json",

        // volumes
        "volumes",
        "volumes/create",
        "volumes/prune",
        "volumes/{name}"
    };

    context.add(std::make_unique<api_handler>(paths, auth));
}


}

int main(int argc, char * argv[])
{
    int exit_code = EXIT_FAILURE;

    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, on_shutdown_requested);
    signal(SIGTERM, on_shutdown_requested);

    try
    {
        authenticator auth;

        webserver_context context;
        add_webcontent(context);
        add_oauth_handlers(context,auth);
        add_api_handlers(context, auth);

        webserver server(8888, std::move(context));

        while (!shutdown_requested) {
            sleep(3);
        }

        exit_code = EXIT_SUCCESS;
    }
    catch (std::exception const & ex)
    {
        std::cout << "error: " << ex.what() << std::endl;        
    }

    return 0;
}