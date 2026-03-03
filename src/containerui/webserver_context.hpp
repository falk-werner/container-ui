#ifndef CONTAINERUI_WEBSERVER_CONTEXT_HPP
#define CONTAINERUI_WEBSERVER_CONTEXT_HPP

#include "containerui/request_handler.hpp"
#include "containerui/auth/authenticator.hpp"

#include <string>
#include <vector>
#include <memory>

namespace container_ui
{

struct webserver_context
{
    void add(std::unique_ptr<request_handler> handler);

    void add_static(
        std::string const & url,
        unsigned char * data,
        unsigned int size,
        std::string const & mimetype);

    void add_passthrough(
        std::string const & path,
        authenticator & auth);

    void add_passthrough_with_param(
        std::string const & url_template,
        std::string const & remote_url_template,
        authenticator & auth,
        std::string const & mimetpye = "application/json");

    std::vector<std::unique_ptr<request_handler>> handlers;
};

}

#endif
