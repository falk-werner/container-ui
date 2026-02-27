#ifndef CONTAINERUI_AUTHORIZE_HANDLER_HPP
#define CONTAINERUI_AUTHORIZE_HANDLER_HPP

#include "containerui/request_handler.hpp"
#include "containerui/authenticator.hpp"

namespace container_ui
{

class authorize_handler: public request_handler
{
public:
    authorize_handler(std::string const & url, authenticator & auth);
    ~authorize_handler() override = default;
        
    bool handle(
        request & req,
        MHD_Result & result) override;

private:
    std::string const _url;
    authenticator & _auth;
};

}

#endif
