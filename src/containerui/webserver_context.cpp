#include "containerui/webserver_context.hpp"
#include "containerui/static_resource_handler.hpp"
#include "containerui/passthrough_handler.hpp"

namespace container_ui
{

void webserver_context::add_static(
    std::string const & url,
    std::string const & contents,
    std::string const & mimetype)
{
    auto handler = std::make_unique<static_resource_handler>(url, contents, mimetype);
    handlers.emplace_back(std::move(handler));
}

void webserver_context::add_passthrough(
    std::string const & url,
    std::string const & remote_url)
{
    auto handler = std::make_unique<passthrough_handler>(url, remote_url);
    handlers.emplace_back(std::move(handler));
}


}