#ifndef CONTAINERUI_REQUEST_HANDLER_HPP
#define CONTAINERUI_REQUEST_HANDLER_HPP

#include <containerui/request.hpp>

namespace container_ui
{

class request_handler
{
public:
    virtual ~request_handler() = default;
    virtual bool handle(
        request & req,
        MHD_Result & result) = 0;
};

}


#endif
