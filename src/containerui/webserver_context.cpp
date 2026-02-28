#include "containerui/webserver_context.hpp"
#include "containerui/static_resource_handler.hpp"
#include "containerui/passthrough_handler.hpp"
#include "containerui/passthrough_with_param_handler.hpp"

namespace container_ui
{

void webserver_context::add(std::unique_ptr<request_handler> handler)
{
    handlers.emplace_back(std::move(handler));
}

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
    std::string const & remote_url,
    authenticator & auth)
{
    auto handler = std::make_unique<passthrough_handler>(url, remote_url, auth);
    handlers.emplace_back(std::move(handler));
}

void webserver_context::add_passthrough_with_param(
    std::string const & url_template,
    std::string const & remote_url_template,
    std::string const & mimetpye)
{
    auto handler = std::make_unique<passthrough_with_param_handler>(url_template, remote_url_template, mimetpye);
    handlers.emplace_back(std::move(handler));
}


}