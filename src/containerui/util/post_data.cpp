#include "containerui/util/post_data.hpp"
#include "containerui/util/url_util.hpp"

#include <sstream>

namespace container_ui
{

namespace
{

struct post_context
{
    size_t limit;
    size_t count;
    std::stringstream contents;
};

}

void parse_post_data(
    std::string const &data,
    std::unordered_map<std::string, std::string> & map)
{
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
            map[key] = value;
        }
        else {
            auto const key = url_decode(chunk);
            map[key] = "";
        }
    }
}


bool update_post_data(
    request & req,
    MHD_Result & result,
    std::string & data)
{
    if ((*req.connection_cls) == nullptr)
    {
        post_context * const context = new post_context;
        context->count = 0;
        context->limit = 100 * 1024;
        *req.connection_cls = reinterpret_cast<void*>(context);

        result = MHD_YES;
        return false;
    }

    post_context * const context = reinterpret_cast<post_context*>(*req.connection_cls);
    auto const size = *req.upload_data_size;

    if (size > 0) {
        size_t const new_count = context->count + size;
        if (new_count > context->limit) {
            delete context;
            *req.connection_cls = nullptr;
            result = MHD_NO;
            return false;
        }

        context->contents.write(req.upload_data, size);
        context->count = new_count;
        *req.upload_data_size = 0;
        result = MHD_YES;
        return false;
    }

    data = context->contents.str();
    delete context;
    *req.connection_cls = nullptr;

    return true;   
}

}