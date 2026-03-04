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

}

path_matcher::path_matcher(std::string const & context, std::vector<std::string> const & paths)
{
    for(auto const & path: paths) {
        path_info info;

        auto const pos = path.find("{name}");
        if (pos == std::string::npos) {
            info.type = matcher_type::simple;
            info.path_or_prefix = context + path;
            info.suffix = "";            
        }
        else {            
            info.type = matcher_type::parameter;
            info.path_or_prefix = context + path.substr(0, pos);
            info.suffix = path.substr(pos + 6);
        }

        path_infos.emplace_back(info);
    }
}

bool path_matcher::matches(std::string const & path) const
{
    for(auto const & info: path_infos) {
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