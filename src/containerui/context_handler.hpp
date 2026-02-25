#ifndef CONTAINERUI_CONTEXT_HANDLER_HPP
#define CONTAINERUI_CONTEXT_HANDLER_HPP

#include <microhttpd.h>
#include <string>

namespace container_ui
{

class context_handler
{
public:
    virtual ~context_handler() = default;
    virtual bool can_handle(std::string const & url) = 0;
    virtual MHD_Result handle(MHD_Connection * connection, std::string const & url) = 0;
};

}


#endif
