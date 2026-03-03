#include "containerui/webserver.hpp"
#include "containerui/webserver_context.hpp"
#include "containerui/webcontent.hpp"
#include "containerui/auth/authorize_handler.hpp"
#include "containerui/auth/token_handler.hpp"
#include "containerui/auth/authenticator.hpp"

#include <unistd.h>

#include <csignal>
#include <iostream>
#include <memory>

using container_ui::webserver;
using container_ui::webserver_context;
using container_ui::authorize_handler;
using container_ui::token_handler;
using container_ui::authenticator;

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
    // sytem
    context.add_passthrough("version", auth);
    context.add_passthrough("info", auth);
    context.add_passthrough("system/df", auth);

    // containers
    context.add_passthrough("containers/json", auth);
    context.add_passthrough_with_param("/api/containers/{name}/logs",
        "http://localhost/containers/{name}/logs?follow=false&stderr=true&stdout=true&timestamps=true",
        auth,
        "text/plain");
    context.add_passthrough_with_param("/api/containers/{name}/json", "http://localhost/containers/{name}/json", auth);
    context.add_passthrough_with_param("/api/containers/{name}/top", "http://localhost/containers/{name}/top?ps_args=-eTopid,ppid,spid,pcpu,pmem,vsz,cmd", auth);
    context.add_passthrough_with_param("/api/containers/{name}/stats", "http://localhost/containers/{name}/stats?oneshot=true&stream=false", auth);

    // images
    context.add_passthrough("images/json", auth);
    context.add_passthrough_with_param("/api/images/{name}/json", "http://localhost/images/{name}/json", auth);

    // volumes
    context.add_passthrough("volumes", auth);
    context.add_passthrough_with_param("/api/volumes/{name}", "http://localhost/volumes/{name}", auth);

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