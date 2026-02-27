#ifndef CONTAINERUI_PASSTHROUGH_WITH_PARAM_HANDLER_HPP
#define CONTAINERUI_PASSTHROUGH_WITH_PARAM_HANDLER_HPP

#include "containerui/request_handler.hpp"

namespace container_ui
{

class passthrough_with_param_handler: public request_handler
{
public:
    passthrough_with_param_handler(
        std::string const & url_template,
        std::string const & remote_url_template,
        std::string const & mimetype);

    ~passthrough_with_param_handler() override = default;

    bool handle(
        request & req,
        MHD_Result & result) override;

private:
    std::string const _mimetype;

    std::string url_prefix;
    std::string url_suffix;
    std::string remote_url_prefix;
    std::string remote_url_suffix;
};

}

#endif
