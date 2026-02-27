#ifndef CONTAINERUI_PASSTHROUGH_HANDLER_HPP
#define CONTAINERUI_PASSTHROUGH_HANDLER_HPP

#include "containerui/request_handler.hpp"

namespace container_ui
{

class passthrough_handler: public request_handler
{
public:
    passthrough_handler(std::string const url, std::string remote_url);

    ~passthrough_handler() override = default;

    bool handle(
        request & req,
        MHD_Result & result) override;

private:
    std::string const _url;
    std::string const _remote_url;
};

}

#endif