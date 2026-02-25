#ifndef CONTAINERUI_WEBSERVER_CONTEXT_HPP
#define CONTAINERUI_WEBSERVER_CONTEXT_HPP

#include "containerui/context_handler.hpp"
#include <string>
#include <vector>
#include <memory>

namespace container_ui
{

struct webserver_context
{
    void add_static(
        std::string const & url,
        std::string const & contents,
        std::string const & mimetype);

    void add_passthrough(
        std::string const & url,
        std::string const & remote_url);

    void add_passthrough_with_param(
        std::string const & url_template,
        std::string const & remote_url_template,
        std::string const & mimetpye = "application/json");

    std::vector<std::unique_ptr<context_handler>> handlers;
};

}

#endif
