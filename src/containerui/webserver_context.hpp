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

    std::vector<std::unique_ptr<context_handler>> handlers;
};

}

#endif
