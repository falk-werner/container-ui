#include "containerui/api/api_handlers.hpp"
#include "containerui/util/fetch.hpp"

#include <stdexcept>

namespace container_ui
{

namespace
{

bool is_valid(char c) {
    return (
        (('0' <= c) && (c <= '9')) ||
        (('a' <= c) && (c <= 'z')) ||
        (('A' <= c) && (c <= 'Z')) ||
        ('-' == c) || ('_' == c)
    );
}

std::string check_url(
    std::string const & url,
    std::string const & prefix,
    std::string const & suffix)
{
    if (!url.starts_with(prefix)) {
        return "";
    }

    if (!url.ends_with(suffix)) {
       return "";
    }

    std::string param = url.substr(prefix.size(), url.size() - prefix.size() - suffix.size());

    for (char c: param) {
        if (!is_valid(c)) {
            return "";
        }
    }

    return param;
}

}

passthrough_with_param_handler::passthrough_with_param_handler(
    std::string const & url_template,
    std::string const & remote_url_template,
    std::string const & mimetype)
: _mimetype(mimetype)
{
    auto param_pos = url_template.find("{name}");
    if (param_pos == std::string::npos) {
        throw std::runtime_error("requirend parameter missing: {name}");
    }
    url_prefix = url_template.substr(0, param_pos);
    url_suffix = url_template.substr(url_prefix.size() + 6);

    param_pos = remote_url_template.find("{name}");
    if (param_pos == std::string::npos) {
        throw std::runtime_error("requirend parameter missing: {name}");
    }
    remote_url_prefix = remote_url_template.substr(0, param_pos);
    remote_url_suffix = remote_url_template.substr(remote_url_prefix.size() + 6);
}



bool passthrough_with_param_handler::handle(
    request & req,
    MHD_Result & result)
{
    std::string const param = check_url(req.url, url_prefix, url_suffix);

    if (param.empty()) { return false; }
    if (req.method != "GET") {
        result = req.respond_empty(MHD_HTTP_METHOD_NOT_ALLOWED);
        return true;
    }

    std::string const remote_url = remote_url_prefix + param + remote_url_suffix;

    auto const resp = fetch(remote_url, "/var/run/docker.sock");
    result = req.respond(resp.status, resp.contents, _mimetype);
    return true;
}

}