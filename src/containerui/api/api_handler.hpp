#ifndef CONTAINERUI_REQUEST_HANDLERS_HPP
#define CONTAINERUI_REQUEST_HANDLERS_HPP

#include "containerui/request_handler.hpp"
#include "containerui/auth/authenticator.hpp"
#include "containerui/util/path_matcher.hpp"

#include <vector>

namespace container_ui
{

class api_handler: public request_handler
{
public:
    api_handler(std::vector<std::string> const & paths, authenticator & auth);
    ~api_handler() override = default;
    bool handle(request & req, MHD_Result & result) override;
private:
    path_matcher matcher;
    authenticator & _auth;
};

}

#endif
