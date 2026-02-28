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
        context.add_static("/", std::string(reinterpret_cast<char*>(index_html), index_html_len), "text/html");
        context.add(std::make_unique<authorize_handler>("/auth/authorize", auth));
        context.add(std::make_unique<token_handler>("/auth/token", auth));

        context.add_passthrough("/api/version", "http://localhost/version", auth);
        context.add_passthrough("/api/info", "http://localhost/info", auth);
        context.add_passthrough("/api/system/df", "http://localhost/system/df", auth);

        context.add_passthrough("/api/containers/json", "http://localhost/containers/json?all=true", auth);
        context.add_passthrough_with_param("/api/containers/{name}/logs",
            "http://localhost/containers/{name}/logs?follow=false&stderr=true&stdout=true&timestamps=true",
            "text/plain");
        context.add_passthrough_with_param("/api/containers/{name}/json", "http://localhost/containers/{name}/json");
        context.add_passthrough_with_param("/api/containers/{name}/top", "http://localhost/containers/{name}/top?ps_args=-eTopid,ppid,spid,pcpu,pmem,vsz,cmd");
        context.add_passthrough_with_param("/api/containers/{name}/stats", "http://localhost/containers/{name}/stats?oneshot=true&stream=false");

        context.add_passthrough("/api/images/json", "http://localhost/images/json?all=true", auth);
        context.add_passthrough_with_param("/api/images/{name}/json", "http://localhost/images/{name}/json");

        context.add_passthrough("/api/volumes", "http://localhost/volumes", auth);
        context.add_passthrough_with_param("/api/volumes/{name}", "http://localhost/volumes/{name}");

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