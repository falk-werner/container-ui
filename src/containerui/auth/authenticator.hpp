#ifndef CONTAINERUI_AUTHENTICATOR_HPP
#define CONTAINERUI_AUTHENTICATOR_HPP

#include <cstdint>
#include <string>
#include <unordered_map>

namespace container_ui
{

struct code_context
{
    uint64_t expire_at;
    std::string challenge;
};

struct token_context
{
    uint64_t expire_at;
};


class authenticator
{
public:
    authenticator();
    ~authenticator() = default;

   std::string authenticate(
        std::string const& response_type,
        std::string const& client_id,
        std::string const& redirect_uri,
        std::string const& scope,
        std::string const& state,
        std::string const& code_challenge_method,
        std::string const& code_challenge,
        std::string const& username,
        std::string const& password);

    std::string get_token(
        std::string const& grant_type,
        std::string const& code,
        std::string const& code_verifier,
        std::string const& client_id,
        std::string const& redirect_uri);

    bool is_token_valid(
        std::string const& token);

private:
    void check_codes_and_tokens();

    std::string const initial_password;
    std::unordered_map<std::string, code_context> codes;
    std::unordered_map<std::string, token_context> tokens;
};

}

#endif
