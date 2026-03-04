#include "containerui/webserver_context.hpp"
#include "containerui/static_resource_handler.hpp"

namespace container_ui
{

void webserver_context::add(std::unique_ptr<request_handler> handler)
{
    handlers.emplace_back(std::move(handler));
}

void webserver_context::add_static(
    std::string const & url,
    unsigned char * data,
    unsigned int size,
    std::string const & mimetype)
{
    std::string contents(reinterpret_cast<char*>(data), size);
    auto handler = std::make_unique<static_resource_handler>(url, contents, mimetype);
    handlers.emplace_back(std::move(handler));
}

}