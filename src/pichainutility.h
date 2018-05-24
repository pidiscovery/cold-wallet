#ifndef PICHAINUTILITY_H
#define PICHAINUTILITY_H

#include <string>
#include <nlohmann/json.hpp>
#include <tuple>

class PiChainUtility
{
public:
    static nlohmann::json Transfer(
            const std::string &sign_key,
            const std::string &from,
            const std::string &to,
            int64_t amount,
            const std::string asset_id,
            const std::string &block_id
    );
    static nlohmann::json Transfer2(
            const std::string &sign_key,
            const std::string &from,
            const std::string &to_pub_key,
            int64_t amount,
            const std::string asset_id,
            const std::string &block_id
    );

    static nlohmann::json VoteForWitness(
            const std::string &sign_key,
            const std::string &acc,
            const std::string &old_options,
            const std::string &vote_id,
            bool vote,
            const std::string &block_id
    );

    static bool IsPublicKey(const std::string &key);
    static std::tuple<std::string, std::string, std::string> GenerateAccount();
};

#endif // PICHAINUTILITY_H
