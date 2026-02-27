#ifndef CONTAINERUI_STATIC_RESOURCE_HANDLER_HPP
#define CONTAINERUI_STATIC_RESOURCE_HANDLER_HPP

#include "containerui/request_handler.hpp"

namespace container_ui
{

class static_resource_handler: public request_handler
{
public:
    static_resource_handler(
        std::string const & url,
        std::string const & contents,
        std::string const & mimetype);
    
    ~static_resource_handler() override = default;

    bool handle(
        request & req,
        MHD_Result & result) override;

private:
    std::string const _url;
    std::string const _contents;
    std::string const _mimetype;
};

}

#endif
