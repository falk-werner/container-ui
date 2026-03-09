#include "containerui/webserver.hpp"
#include "containerui/webserver_context.hpp"
#include "containerui/webcontent.hpp"
#include "containerui/auth/authorize_handler.hpp"
#include "containerui/auth/token_handler.hpp"
#include "containerui/auth/authenticator.hpp"
#include "containerui/api/api_handler.hpp"
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
using container_ui::matcher_type;
using container_ui::matcher_verb;
using container_ui::path_info;

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
    std::vector<path_info> paths = {
        // system
        {matcher_type::simple, "version", "", matcher_verb::get},
        {matcher_type::simple, "info", "", matcher_verb::get},
        {matcher_type::simple, "system/df", "", matcher_verb::get},

        // containers
        {matcher_type::simple, "containers/json", "", matcher_verb::get},
        {matcher_type::parameter, "containers/", "/logs", matcher_verb::get},
        {matcher_type::parameter, "containers/", "/json", matcher_verb::get},
        {matcher_type::parameter, "containers/", "/top", matcher_verb::get},
        {matcher_type::parameter, "containers/", "/stats", matcher_verb::get},
        {matcher_type::parameter, "containers/", "/start", matcher_verb::post},
        {matcher_type::parameter, "containers/", "/stop", matcher_verb::post},
        {matcher_type::parameter, "containers/", "/pause",matcher_verb::post},
        {matcher_type::parameter, "containers/", "/unpause",matcher_verb::post},
        {matcher_type::parameter, "containers/", "/restart",matcher_verb::post},
        {matcher_type::parameter, "containers/", "/kill",matcher_verb::post},
        {matcher_type::parameter, "containers/", "",matcher_verb::del},

        // images
        {matcher_type::simple, "images/json", "", matcher_verb::get},
        {matcher_type::parameter, "images/", "/json", matcher_verb::get},
        {matcher_type::parameter, "images/", "", matcher_verb::del},

        // volumes
        {matcher_type::simple, "volumes", "", matcher_verb::get},
        {matcher_type::simple, "volumes/create", "", matcher_verb::post},
        {matcher_type::simple, "volumes/prune", "", matcher_verb::post},
        {matcher_type::parameter, "volumes/", "", matcher_verb::get_del}
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