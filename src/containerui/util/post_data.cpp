#include "containerui/util/post_data.hpp"
#include "containerui/util/url_util.hpp"

#include <sstream>

namespace container_ui
{

std::unordered_map<std::string, std::string> parse_post_data(std::string const &data)
{
    std::unordered_map<std::string, std::string> parsed;

    std::string chunk;
    std::stringstream stream(data);

    while (std::getline(stream, chunk, '&'))
    {
        if (chunk.empty()) {
            continue;
        }

        auto const pos = chunk.find('=');
        if (pos != std::string::npos) {
            auto const key = url_decode(chunk.substr(0, pos));
            auto const value = url_decode(chunk.substr(pos + 1));
            parsed[key] = value;
        }
        else {
            auto const key = url_decode(chunk);
            parsed[key] = "";
        }
    }

    return parsed;
}

}