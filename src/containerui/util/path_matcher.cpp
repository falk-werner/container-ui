#include "containerui/util/path_matcher.hpp"

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

matcher_verb get_verb(request const & req) {
    if (req.method == "GET") {
        return matcher_verb::get;
    }
    if (req.method == "POST") {
        return matcher_verb::post;
    }
    if (req.method == "DELETE") {
        return matcher_verb::del;
    }
    return matcher_verb::none;
}

}

path_matcher::path_matcher(std::string const & context, std::vector<path_info> const & paths)
: path_infos(paths)
{
    for(auto & info: path_infos) {
        info.path_or_prefix = context + info.path_or_prefix;
    }
}

bool path_matcher::matches(request const & req) const
{
    auto const & path = req.url;
    auto const verb = static_cast<int>(get_verb(req));

    for(auto const & info: path_infos) {
        if ((static_cast<int>(info.verb) & verb) == 0) {
            continue;
        }

        if (info.type == matcher_type::simple) {
            if (path == info.path_or_prefix) {
                return true;
            }
        }
        else { 
            if (!path.starts_with(info.path_or_prefix)) {
                continue;
            }

            if (!path.ends_with(info.suffix)) {
                continue;
            }

            auto const start = info.path_or_prefix.size();
            auto const size = path.size() - start - info.suffix.size();
            std::string const param = path.substr(start, size);

            bool valid = true;
            for (char c: param) {
                if (!is_valid(c)) {
                    valid = false;
                    break;
                }
            }

            if (valid) {
                return true;
            }
        }
    }

    return false;
}


}