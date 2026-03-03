#ifndef CONTAINERUI_REQUEST_HANDLERS_HPP
#define CONTAINERUI_REQUEST_HANDLERS_HPP

#include "containerui/request_handler.hpp"
#include "containerui/auth/authenticator.hpp"

namespace container_ui
{

class passthrough_handler: public request_handler
{
public:
    passthrough_handler(
        std::string const url,
        std::string remote_url,
        authenticator & auth);

    ~passthrough_handler() override = default;

    bool handle(
        request & req,
        MHD_Result & result) override;

private:
    std::string const _url;
    std::string const _remote_url;
    authenticator & _auth;
};

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
