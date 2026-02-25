#ifndef CONTAINERUI_PASSTHROUGH_HANDLER_HPP
#define CONTAINERUI_PASSTHROUGH_HANDLER_HPP

#include "containerui/context_handler.hpp"

namespace container_ui
{

class passthrough_handler: public context_handler
{
public:
    passthrough_handler(std::string const url, std::string remote_url);

    ~passthrough_handler() override = default;

    bool can_handle(std::string const & url) override;

    MHD_Result handle(MHD_Connection * connection, std::string const & url) override;

private:
    std::string const _url;
    std::string const _remote_url;
};

}

#endif