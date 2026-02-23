#include "containerui/webserver.hpp"
#include <stdexcept>

namespace container_ui
{

namespace
{

MHD_Result webserver_handle_request(
    void * cls,
    MHD_Connection * connection,
    char const * url,
    char const * method,
    char const * version,
    char const * upload_data,
    size_t * upload_data_size,
    void * * connection_cls)
{
    if (method != std::string("GET"))
    {
        return MHD_NO;
    }

    auto * context = reinterpret_cast<webserver_context*>(cls);
    if (context == nullptr)
    {
        return MHD_NO;
    }

    try
    {
        for(auto const &handler: context->handlers)
        {
            if (handler->can_handle(url))
            {
                return handler->handle(connection);
            }
        }
    }
    catch(...)
    {
        return MHD_NO;
    }

    return MHD_NO;
}

}

webserver::webserver(uint16_t port, webserver_context && context)
: _context(std::move(context))
{
    daemon = MHD_start_daemon(
        MHD_USE_INTERNAL_POLLING_THREAD, port,
        nullptr, nullptr,
        &webserver_handle_request, reinterpret_cast<void*>(&_context),
        MHD_OPTION_END);

    if (daemon == nullptr)
    {
        throw std::runtime_error("failed to start webserver");
    }
}

webserver::~webserver()
{
    MHD_stop_daemon(daemon);
}



}