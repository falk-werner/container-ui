#include "containerui/auth/authenticator.hpp"
#include "containerui/util/timestamp.hpp"
#include "containerui/util/token.hpp"
#include "containerui/util/b64.hpp"
#include "containerui/util/sha256.hpp"

#include <iostream>
#include <algorithm>
#include <sstream>

namespace container_ui
{

authenticator::authenticator()
: initial_password(generate_token())
{
    std::cout << "initial admin password: " << initial_password << std::endl;
}

std::string authenticator::authenticate(
    std::string const& response_type,
    std::string const& client_id,
    std::string const& redirect_uri,
    std::string const& scope,
    std::string const& state,
    std::string const& code_challenge_method,
    std::string const& code_challenge,
    std::string const& username,
    std::string const& password)
{
    check_codes_and_tokens();

    if (codes.size() > 100) {
        std::cerr << "max number of codes exceeded";
        return "";
    }

    if (redirect_uri != "/") {
        std::cerr << "invalid redirect_uri";
        return "";
    }

    if (response_type != "code") {
        return "/?error=invalid%20response_type";
    }

    if (client_id != "container_ui") {
        return "/?error=invalid%20client_id";
    }

    if (scope != "container_ui") {
        return "/?error=invalid%20scope";
    }

    if (code_challenge_method != "S256") {
        return "/?error=unknown%20code_challenge_method";
    }

    if ((username != "admin") || (password != initial_password))
    {
        return "/?error=authentication%20failed";
    }

    std::string code = generate_token();
    while (codes.contains(code)) {
        code = generate_token();
    }

    code_context context;
    context.expire_at = get_timestamp() + 60;
    context.challenge = code_challenge;

    codes[code] = context;
    return std::string("/?code=") + code + "&state=" + state;
}

std::string authenticator::get_token(
    std::string const& grant_type,
    std::string const& code,
    std::string const& code_verifier,
    std::string const& client_id,
    std::string const& redirect_uri)
{
    check_codes_and_tokens();

    if (tokens.size() > 100) {
        return "{\"error\":\"too many active tokens - try again later\"}";
    }

    if (grant_type != "authorization_code") {
        return "{\"error\":\"invalid grant_type\"}";
    }

    if (redirect_uri != "/") {
        return "{\"error\":\"invalid redirect_uri\"}";
    }

    if (client_id != "container_ui") {
        return "{\"error\":\"invalid client_id\"}";
    }

    auto entry = codes.find(code);
    if (entry == codes.end()) {
        return "{\"error\":\"invalid code\"}";
    }

    auto const computed_challenge = b64url_encode(sha256(code_verifier));
    if (computed_challenge != entry->second.challenge) {
        codes.erase(entry);
        return "{\"error\":\"invalid code verifier\"}";
    }

    codes.erase(entry);

    std::string token = generate_token();
    while (tokens.contains(token)) {
        token = generate_token();
    }
    token_context context;
    context.expire_at = get_timestamp() + (2 * 60 * 60);
    tokens[token] = context;

    std::stringstream response;
    response 
        << "{"
            << "\"token_type\":\"Bearer\","
            << "\"access_token\":\"" << token << "\","
            << "\"expires_in\":" << (2 * 60 * 60) << ","
            << "\"scope\":\"container_ui\""
        << "}";

    return response.str();
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
        return (now > entry.second.expire_at);
    });

    std::erase_if(tokens, [now](auto & entry){
        return (now > entry.second.expire_at);
    });
}

}
