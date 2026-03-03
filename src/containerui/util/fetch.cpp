#include "containerui/util/fetch.hpp"
#include <curl/curl.h>
#include <cstring>
#include <sstream>
#include <stdexcept>

namespace container_ui
{

namespace
{

struct read_context
{
    char const * data;
    size_t size;
    size_t pos;
};

struct write_context
{
    size_t count;
    size_t limit;
    std::stringstream buffer;
};

static size_t on_read(void *ptr, size_t size, size_t n, void *userdata)
{
    auto * context = reinterpret_cast<read_context*>(userdata);
    if (nullptr == context) {
        return CURL_WRITEFUNC_ERROR;
    }

    size_t const count = n * size;
    size_t const remaining = context->size - context->pos;
    size_t const bytes_to_read = (remaining < count) ? remaining : count;

    if (bytes_to_read > 0)
    {
        char const * data = &(context->data[context->pos]);
        memcpy(ptr, reinterpret_cast<void const*>(data), bytes_to_read);
        context->pos += bytes_to_read;
    }

    return bytes_to_read;

}

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

http_response post(
    std::string const & url,
    std::string const & unix_socket,
    std::string const & data,
    std::string const & mimetype)
{
    read_context r_context;
    r_context.data = data.c_str();
    r_context.size = data.size();
    r_context.pos = 0;

    write_context context;
    context.limit = 1 * 1024 * 1024;
    context.count = 0;

    auto * curl = curl_easy_init();

    std::string content_type = std::string("Content-Type: ") + mimetype;
    curl_slist *list = nullptr;
    list = curl_slist_append(list, content_type.c_str());

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_UNIX_SOCKET_PATH, unix_socket.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, &on_read);
    curl_easy_setopt(curl, CURLOPT_READDATA, reinterpret_cast<void*>(&r_context));
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &on_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, reinterpret_cast<void*>(&context));
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5);

    auto const code = curl_easy_perform(curl);
    curl_slist_free_all(list);
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