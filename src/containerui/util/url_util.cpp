#include "containerui/util/url_util.hpp"
#include <curl/curl.h>

namespace container_ui
{

std::string url_encode(std::string const & value)
{
    auto * result_str = curl_escape(value.c_str(), value.size());
    if (nullptr == result_str) {
        return "";
    }

    std::string result = result_str;
    curl_free(result_str);
    return result;
}

std::string url_decode(std::string const & value)
{
    auto * result_str = curl_unescape(value.c_str(), value.size());
    if (nullptr == result_str) {
        return "";
    }

    std::string result = result_str;
    curl_free(result_str);
    return result;
}


}