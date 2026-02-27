#ifndef CONTAINERUI_POST_DATA_HPP
#define CONTAINERUI_POST_DATA_HPP

#include <string>
#include <unordered_map>

namespace container_ui
{

std::unordered_map<std::string, std::string> parse_post_data(std::string const &data);

}

#endif
