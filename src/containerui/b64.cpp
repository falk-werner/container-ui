#include "containerui/b64.hpp"

#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>

#include <algorithm>
#include <sstream>

namespace container_ui
{

std::string b64_encode(std::string const data)
{
    BIO *bio, *b64;
	BUF_MEM *bufferPtr;

	b64 = BIO_new(BIO_f_base64());
	bio = BIO_new(BIO_s_mem());
	bio = BIO_push(b64, bio);

	BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
	BIO_write(bio, data.c_str(), data.size());
	BIO_flush(bio);
	BIO_get_mem_ptr(bio, &bufferPtr);
	BIO_set_close(bio, BIO_NOCLOSE);
	BIO_free_all(bio);

	std::string result(bufferPtr->data, bufferPtr->length);
    BUF_MEM_free(bufferPtr);

    return result;
}

std::string b64_decode(std::string const data)
{
    BIO *bio, *b64;
    char inbuf[512];
    int inlen;

    b64 = BIO_new(BIO_f_base64());    
	BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    bio = BIO_new_mem_buf(data.c_str(), data.size());
    BIO_push(b64, bio);
    
    std::stringstream stream;
    while ((inlen = BIO_read(b64, inbuf, 512)) > 0) {
        stream.write(inbuf, inlen);
    }

    BIO_free_all(b64);

    return stream.str();
}

std::string b64url_encode(std::string const data)
{
    std::string result = b64_encode(data);
    for(auto & c: result) {
        switch (c) {
            case '+':
                c = '-';
                break;
            case '/':
                c = '_';
                break;
            default:
                break;
        }
    }

    std::erase_if(result, [](auto c) { return (c == '='); });

    return result;
}

std::string b64url_decode(std::string const data)
{
    std::string result = data;
    for(auto & c: result) {
        switch (c) {
            case '-':
                c = '+';
                break;
            case '_':
                c = '/';
                break;
            default:
                break;
        }
    }

    while ((result.size() % 4) != 0) {
        result += '=';
    }

    return b64_decode(result);
}


}