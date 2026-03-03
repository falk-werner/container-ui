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
        std::string const & path,
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
        std::string const & mimetype,
        authenticator & _auth);

    ~passthrough_with_param_handler() override = default;

    bool handle(
        request & req,
        MHD_Result & result) override;

private:
    std::string const _mimetype;
    authenticator & _auth;

    std::string url_prefix;
    std::string url_suffix;
    std::string remote_url_prefix;
    std::string remote_url_suffix;
};

class create_volume_handler: public request_handler
{
public:
    explicit create_volume_handler(authenticator & auth);
    ~create_volume_handler() override = default;
    bool handle(
        request & req,
        MHD_Result & result) override;
private:
    authenticator & _auth;
};

}

#endif
