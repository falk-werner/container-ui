#include "containerui/authenticator.hpp"
#include "containerui/timestamp.hpp"

#include <iostream>
#include <algorithm>

namespace container_ui
{

authenticator::authenticator()
: initial_password("secret")
{
    std::cout << "initial admin password: " << initial_password << std::endl;
}

std::string authenticator::authenticate(
    std::string const& username,
    std::string const& password,
    std::string const& challenge)
{
    check_codes_and_tokens();

    if ((username == "admin") && (password == initial_password))
    {
        std::string const code = "code";
        code_context context;
        context.expire_at = get_timestamp() + 60;
        context.challenge = challenge;

        codes[code] = context;
        return code;
    }

    return "";
}

std::string authenticator::get_token(
    std::string const& code,
    std::string const& code_verifier)
{
    check_codes_and_tokens();

    auto entry = codes.find(code);
    if (entry != codes.end()) {
        codes.erase(entry);

        std::string const token = "token";
        token_context context;
        context.expire_at = get_timestamp() + (2 * 60 * 60);
        return token;
    }

    return "";
}

bool authenticator::is_token_valid(
    std::string const& token)
{
    check_codes_and_tokens();

    auto entry = tokens.find(token);
    return (entry != tokens.end());
}

void authenticator::check_codes_and_tokens()
{
    auto now = get_timestamp();

    std::erase_if(codes, [now](auto & entry){
        return (now < entry.second.expire_at);
    });

    std::erase_if(tokens, [now](auto & entry){
        return (now < entry.second.expire_at);
    });
}

}
