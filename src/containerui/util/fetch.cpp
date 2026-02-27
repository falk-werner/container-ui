#include "containerui/util/fetch.hpp"
#include <curl/curl.h>
#include <sstream>
#include <stdexcept>

namespace container_ui
{

namespace
{

struct write_context
{
    size_t count;
    size_t limit;
    std::stringstream buffer;
};

size_t on_data(char * ptr, size_t size, size_t n, void * userdata)
{
    auto * context = reinterpret_cast<write_context*>(userdata);
    if (nullptr == context) {
        return CURL_WRITEFUNC_ERROR;
    }

    size_t const count = n * size;
    if ((context->count + count) > context->limit) {
        return CURL_WRITEFUNC_ERROR;
    }

    context->buffer.write(ptr, count);
    context->count += count;
    return n;
}

}

http_response fetch(
    std::string const & url,
    std::string const & unix_socket)
{
    write_context context;
    context.limit = 1 * 1024 * 1024;
    context.count = 0;

    auto * curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_UNIX_SOCKET_PATH, unix_socket.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &on_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, reinterpret_cast<void*>(&context));
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5);

    auto const code = curl_easy_perform(curl);
    if (code != CURLE_OK) {
        curl_easy_cleanup(curl);
        throw std::runtime_error(curl_easy_strerror(code));
    }

    long status = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status);    
    curl_easy_cleanup(curl);

    return { static_cast<int>(status), context.buffer.str() };
}


}