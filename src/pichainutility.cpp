#include "pichainutility.h"

#include <graphene/chain/database.hpp>
#include <graphene/chain/balance_object.hpp>
#include <graphene/chain/asset_object.hpp>
#include <graphene/chain/witness_object.hpp>
#include <graphene/chain/committee_member_object.hpp>
#include <graphene/chain/proposal_object.hpp>
#include <graphene/utilities/key_conversion.hpp>
#include <graphene/chain/protocol/operations.hpp>
#include <fc/crypto/base36.hpp>
#include <QDebug>
#include <QUuid>

nlohmann::json PiChainUtility::Transfer2(
        const std::string &sign_key,
        const std::string &from,
        const std::string &to_pub_key,
        int64_t amount,
        const std::string asset_id,
        const std::string &block_id) {
    graphene::chain::account_create_by_transfer_operation op;
    op.fee = graphene::chain::asset(1000);

    op.account_key = graphene::chain::public_key_type(to_pub_key);
    fc::from_variant(from, op.from);

    op.amount.amount = amount;
    fc::from_variant(asset_id, op.amount.asset_id);

    graphene::chain::signed_transaction tx;
    tx.set_reference_block(graphene::chain::block_id_type(block_id));
    tx.set_expiration(fc::time_point_sec(fc::time_point::now()) + 30);
    tx.operations.push_back(op);

    tx.sign(*graphene::utilities::wif_to_key(sign_key), graphene::chain::chain_id_type("ae471be89b3509bf7474710dda6bf35d893387bae70402b54b616d72b83bc5a4"));

    return nlohmann::json::parse(fc::json::to_string(tx));
}


nlohmann::json PiChainUtility::Transfer(
        const std::string &sign_key,
        const std::string &from,
        const std::string &to,
        int64_t amount,
        const std::string asset_id,
        const std::string &block_id) {
    graphene::chain::transfer_operation op;
    op.fee = graphene::chain::asset(100);

    fc::from_variant(from, op.from);
    fc::from_variant(to, op.to);

    op.amount.amount = amount;
    fc::from_variant(asset_id, op.amount.asset_id);

    graphene::chain::signed_transaction tx;
    tx.set_reference_block(graphene::chain::block_id_type(block_id));
    tx.set_expiration(fc::time_point_sec(fc::time_point::now()) + 30);
    tx.operations.push_back(op);

    tx.sign(*graphene::utilities::wif_to_key(sign_key), graphene::chain::chain_id_type("ae471be89b3509bf7474710dda6bf35d893387bae70402b54b616d72b83bc5a4"));

    return nlohmann::json::parse(fc::json::to_string(tx));
}

bool PiChainUtility::IsPublicKey(const std::string &key) {
    return key.size() > 3 && key[0] =='P' && key[1] == 'I' && key[2] == 'C';
//    try {
//        // TODO seems not work
//        auto pub_key = graphene::chain::public_key_type(key);
//        if (pub_key == graphene::chain::public_key_type()) {
//            return false;
//        }
//        return true;
//    } catch (...) {
//        return false;
//    }
}

std::tuple<std::string, std::string, std::string> PiChainUtility::GenerateAccount() {
    std::string seed;
    for (int i = 0; i < 32; i++) {
        seed += QUuid::createUuid().toString().toStdString();
    }
    auto key = fc::ecc::private_key::regenerate(fc::sha256::hash(seed));
    auto pub_key = key.get_public_key();
    auto bts_pub_key = graphene::chain::public_key_type(pub_key);

    auto key_data = pub_key.serialize();
    auto base36 = fc::to_base36(key_data.data, key_data.size());
    auto name = "n" + base36;

    auto pri_key = graphene::utilities::key_to_wif(key);

    return std::tuple<std::string, std::string, std::string>(name, bts_pub_key, pri_key);
}

