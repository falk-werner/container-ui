#ifndef CONTAINERUI_POST_DATA_HPP
#define CONTAINERUI_POST_DATA_HPP

#include "containerui/request.hpp"

#include <string>
#include <unordered_map>


namespace container_ui
{

void parse_post_data(
    std::string const &data,
    std::unordered_map<std::string, std::string> & map);

bool update_post_data(
    request & req,
    MHD_Result & result,
    std::string & data);

}

#endif
