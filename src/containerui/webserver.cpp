#include "containerui/webserver.hpp"
#include <stdexcept>
#include <iostream>

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
    auto * context = reinterpret_cast<webserver_context*>(cls);
    if (context == nullptr)
    {
        return MHD_NO;
    }

    request req {
        connection, url, method,
        upload_data, upload_data_size,
        connection_cls
    };

    try
    {
        for(auto const &handler: context->handlers)
        {
            MHD_Result result = MHD_NO;
            if (handler->handle(req, result))
            {
                return result;
            }
        }

        auto * response = MHD_create_response_empty(MHD_RF_NONE);
        auto result = MHD_queue_response(connection, MHD_HTTP_NOT_FOUND, response);
        MHD_destroy_response(response);
        return result;
    }
    catch(std::exception const & ex)
    {
        std::cerr << "request failed: " << method << " " << url << ": " << ex.what() << std::endl; 
        return MHD_NO;
    }
    catch(...)
    {
        std::cerr << "request failed: " << method << " " << url << std::endl; 
        return MHD_NO;
    }
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