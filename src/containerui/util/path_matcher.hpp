#ifndef CONTAINERUI_PATH_MATCHER_HPP
#define CONTAINERUI_PATH_MATCHER_HPP

#include <vector>
#include <string>

namespace container_ui
{

enum class matcher_type
{
    simple,
    parameter
};

struct path_info
{
    matcher_type type;
    std::string path_or_prefix;
    std::string suffix;
};

class path_matcher
{
public:
    path_matcher(std::string const & context, std::vector<std::string> const & paths);
    ~path_matcher() = default;
    bool matches(std::string const & path) const;
private:
    std::vector<path_info> path_infos;
};

}

#endif
