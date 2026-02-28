#ifndef CONTAINERUI_TOKEN_HANDLER_HPP
#define CONTAINERUI_TOKEN_HANDLER_HPP

#include <containerui/request_handler.hpp>
#include <containerui/auth/authenticator.hpp>

#include <string>

namespace  container_ui
{

class token_handler: public request_handler
{
public:
    token_handler(std::string const & url, authenticator & auth);
    ~token_handler() override = default;

    bool handle(request & req, MHD_Result & result) override;

private:
    std::string const _url;
    authenticator & _auth;
};

}

#endif
