#ifndef CONTAINERUI_PATH_MATCHER_HPP
#define CONTAINERUI_PATH_MATCHER_HPP

#include "containerui/request.hpp"
#include <vector>
#include <string>

namespace container_ui
{

enum class matcher_type
{
    simple,
    parameter
};

enum class matcher_verb: int
{
    none = 0,
    get = 1,
    post = 2,
    del = 4,
    get_del = get + del
};

struct path_info
{
    matcher_type type;
    std::string path_or_prefix;
    std::string suffix;
    matcher_verb verb;
};

class path_matcher
{
public:
    path_matcher(std::string const & context, std::vector<path_info> const & paths);
    ~path_matcher() = default;
    bool matches(request const & req) const;
private:
    std::vector<path_info> path_infos;
};

}

#endif
