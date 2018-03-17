#include "piwebsocketclient.h"
#include <QDebug>
#include <set>
#include <pichainutility.h>

using json = nlohmann::json;


PiWebsocketClient::PiWebsocketClient(QObject *parent) : QObject(parent), is_connect(false), is_login(false), inc_id(0)
{
    websocket_client = std::make_shared<QWebSocket>();
    connect(websocket_client.get(), &QWebSocket::connected, [this](){
        qDebug() << "connected";
        this->is_connect = true;
        this->Login([this](bool ok){
            qDebug() << "login: " << ok ;
            this->is_login = ok;
            emit this->login(ok);
        });
    });

    connect(websocket_client.get(), &QWebSocket::disconnected, [this](){
        qDebug() << "disconnected";
        this->is_connect = false;
        this->is_login = false;
    });

    connect(websocket_client.get(), &QWebSocket::textMessageReceived, [this](const QString &message){
        qDebug() << "message received";
        qDebug() << message;
        try {
            auto j = json::parse(message.toStdString());
            int id = j["id"];
            auto it = this->cb_map.find(id);
            qDebug() << "callback: " << id;
            if (it != this->cb_map.end()) {
                if (j.find("result") != j.end()) {
                    it->second(true, j["result"]);
                } else if (j.find("result") != j.end()) {
                    it->second(true, j["error"]);
                }
                this->cb_map.erase(it);
            }
        } catch (...) {
            qDebug() << "parse message json fail.";
        }

    });

    websocket_client->open(QUrl("ws://cold.piex.pro:8010"));
}

void PiWebsocketClient::Login(std::function<void(bool)> cb) {
    auto j = json::array();
    j.push_back("");
    j.push_back("");
    auto id = GetNextId();
    auto data = BuildRequestJson("login", id, j);

    qDebug() << data.c_str();

    if (!CallServer(id, data, [cb](bool ok, const json& res){
        if (ok && res.is_boolean() && res.get<bool>() == true) {
            qDebug() << "login ok";
            cb(true);
        } else {
            cb(false);
        }
    })) {
        cb(false);
    }
}

int PiWebsocketClient::GetApiId(const std::string &api) {
    static const std::set<std::string> database = {
        "get_chain_properties",
        "get_account_balances",
        "get_account",
        "get_account_by_name",
        "get_dynamic_global_properties",
        "get_key_references",
        "get_objects",
    };
    static const std::set<std::string> history = {
        "get_account_history",
    };
    static const std::set<std::string> network_broadcast = {
        "broadcast_transaction",
    };

    for (auto it : api_map) {
        qDebug() << it.first.c_str() << ": " << it.second;
    }
    if (database.find(api) != database.end()) {
        return api_map["database"];
    }
    if (history.find(api) != history.end()) {
        return api_map["history"];
    }
    if (network_broadcast.find(api) != network_broadcast.end()) {
        return api_map["network_broadcast"];
    }
    return 1;
}

void PiWebsocketClient::ApiRegister(const std::string &api_type, std::function<void (bool)> callback) {
    auto j = json::array();
    auto id = GetNextId();
    auto data = BuildRequestJson(api_type, id, j);
    auto cb = callback;
    if (!CallServer(id, data, [this, api_type, cb](bool, const json &res){
        if (res.is_number_unsigned()) {
            this->api_map[api_type] = res.get<int>();
            cb(true);
        } else {
            cb(false);
        }
    })) {
        callback(false);
    }
}

void PiWebsocketClient::GetDynamiGlobalProperties(std::function<void (bool, const json&)> cb) {
    auto j = json::array();
    auto id = GetNextId();
    auto data = BuildRequestJson("get_dynamic_global_properties", id, j);

    if (!CallServer(id, data, [cb](bool, const json &res){
        cb(true, res);
    })) {
        cb(false, json({}));
    }
}

void PiWebsocketClient::GetAccountIdByName(const std::string &name, std::function<void (bool, const std::string&)> cb) {
    auto j = json::array();
    j.push_back(name);
    auto id = GetNextId();
    auto data = BuildRequestJson("get_account_by_name", id, j);

    if (!CallServer(id, data, [cb](bool, const json &res){
        try {
            if (res.is_null()) {
                cb(false, "");
                return;
            }
            cb(true, res["id"].get<std::string>());
        } catch (...) {
            cb(false, "");
        }
    })) {
        cb(false, "");
    }
}

void PiWebsocketClient::GetAccountByName(const std::string &name, std::function<void (bool, const nlohmann::json&)> cb) {
    auto j = json::array();
    j.push_back(name);
    auto id = GetNextId();
    auto data = BuildRequestJson("get_account_by_name", id, j);

    if (!CallServer(id, data, [cb](bool, const json &res){
        cb(true, res);
    })) {
        cb(false, json({}));
    }
}

void PiWebsocketClient::GetAccountNameById(const std::string &acc_id, std::function<void(bool ok, const std::string&)> cb) {
    auto j = json::array();
    auto ids = json::array();
    ids.push_back(acc_id);
    j.push_back(ids);
    auto id = GetNextId();
    auto data = BuildRequestJson("get_objects", id, j);

    if (!CallServer(id, data, [cb](bool, const json &res){
        try {
            std::string name = res[0]["name"].get<std::string>();
            cb(true, name);
        } catch (...) {
            cb(false, "");
        }
    })) {
        cb(false, "");
    }
}

void PiWebsocketClient::GetAccountNameByPublicKey(const std::string &key, std::function<void(bool ok, const std::string&)> cb) {
    auto j = json::array();
    auto keys = json::array();
    keys.push_back(key);
    j.push_back(keys);

    auto id = GetNextId();
    auto data = BuildRequestJson("get_key_references", id, j);

    qDebug() << data.c_str();

    if (!CallServer(id, data, [this, cb](bool, const json &res){
        qDebug() << res.dump(2).c_str();
        try {
            if (!res.is_array() || res.size() < 1 || !res[0].is_array() || res[0].size() < 1 || !res[0][0].is_string()) {
                cb(false, "");
                return;
            }
            std::string acc_id = res[0][0].get<std::string>();
            this->GetAccountNameById(acc_id, [cb](bool ok, const std::string &name){
                if (ok) {
                    cb(true, name);
                } else {
                    cb(false, "");
                }
            });
        } catch (...) {
            cb(false, "");
        }
    })) {
        cb(false, "");
    }
}

void PiWebsocketClient::GetBalance(const std::string &name, std::function<void(bool, uint64_t)> cb) {
    GetAccountIdByName(name, [this, cb](bool ok, const std::string &id){
        if (ok) {
            auto j = json::array();
            j.push_back(id);
            auto assets = json::array();
            assets.push_back("1.3.0");
            j.push_back(assets);
            auto id = this->GetNextId();
            auto data = this->BuildRequestJson("get_account_balances", id, j);
            if (!this->CallServer(id, data, [cb](bool ok, const json &res){
                try {
//                    qDebug() << "balances:" << res[0]["amount"].get<uint64_t>();
                    cb(true, res[0]["amount"].get<uint64_t>());
                } catch (...) {
                    cb(false, 0);
                }
            })) {
                cb(false, 0);
            }
        } else {
            cb(false, 0);
        }
    });
}

void PiWebsocketClient::GetAccountHistory(const std::string &name, int stop, int num, int start, std::function<void (bool, const nlohmann::json&)> cb) {
    GetAccountIdByName(name, [this, &name, &stop, &num, &start, cb](bool ok, const std::string &id){
        if (ok) {
            if (stop < 0) stop = 0;
            if (num <= 0) num = 1;
            if (num > 100) num = 100;
            if (start < 0 || start == stop) start = 0;
            auto j = json::array();
            j.push_back(id);
            j.push_back("1.11." + std::to_string(stop));
            j.push_back(num);
            j.push_back("1.11." + std::to_string(start));
            auto id = this->GetNextId();
            auto data = this->BuildRequestJson("get_account_history", id, j);
            if (!this->CallServer(id, data, [cb](bool ok, const json &res){
                cb(ok, res);
            })) {
                cb(false, json({}));
            }
        } else {
            cb(false, json({}));
        }
    });
}

std::string PiWebsocketClient::BuildRequestJson(const std::string &api, int id, const json &p) {
    Q_ASSERT(p.is_array());
    json j;
    j["method"] = "call";
    j["id"] = id;
    auto params = json::array();
    params.push_back(GetApiId(api));
    params.push_back(api);
    params.push_back(p);
    j["params"] = params;
    return j.dump();
}

int PiWebsocketClient::GetNextId() {
    return ++inc_id;
}

bool PiWebsocketClient::CallServer(int id, const std::string &data, std::function<void(bool, const json&)> cb) {
    if (!is_connect) {
        return false;
    }
    cb_map[id] = std::move(cb);
    websocket_client->sendTextMessage(data.c_str());
    return true;
}

void PiWebsocketClient::BroadcastTransaction(const json &tx, std::function<void(bool, const nlohmann::json&)> cb) {
    auto j = json::array();
    j.push_back(tx);
    auto id = GetNextId();
    auto data = BuildRequestJson("broadcast_transaction", id, j);
    qDebug() << data.c_str();
    if (!CallServer(id, data, [cb](bool, const json &res){
        cb(true, res);
    })) {
        cb(false, json({}));
    }
}

bool PiWebsocketClient::IsReady() {
    return is_connect && is_login;
}

void PiWebsocketClient::TransferTo(
        const std::string sign_key,
        const std::string &from,
        const std::string &to,
        uint64_t amount,
        const std::string &asset,
        std::function<void(bool)> cb) {
    GetAccountIdByName(from, [this, sign_key, to, amount, cb](bool ok, const std::string &id){
        if (ok) {
            std::string from_id = id;
            this->GetAccountIdByName(to, [this, sign_key, from_id, amount, cb](bool ok, const std::string &id){
                if (ok) {
                    std::string to_id = id;
                    this->GetDynamiGlobalProperties([this, sign_key, from_id, amount, to_id, cb](bool ok, const json &d){
                        if (ok) {
                            try {
                                std::string block_id = d["head_block_id"].get<std::string>();
                                auto tx = PiChainUtility::Transfer(sign_key, from_id, to_id, amount, "1.3.0", block_id);
                                qDebug() << tx.dump(2).c_str();
                                this->BroadcastTransaction(tx, [cb](bool ok, const json &d) {
                                    qDebug() << d.dump(2).c_str();
                                    cb(ok);
                                });
                            } catch (...) {
                                cb(false);
                            }
                        } else {
                            cb(false);
                        }
                    });
                } else {
                    cb(false);
                }
            });
        } else {
            cb(false);
        }
    });
}

void PiWebsocketClient::TransferTo2(
        const std::string sign_key,
        const std::string &from,
        const std::string &to_pub_key,
        uint64_t amount,
        const std::string &asset,
        std::function<void(bool)> cb) {
    GetAccountIdByName(from, [this, sign_key, to_pub_key, amount, cb](bool ok, const std::string &id){
        if (ok) {
            std::string from_id = id;
                this->GetDynamiGlobalProperties([this, sign_key, from_id, amount, to_pub_key, cb](bool ok, const json &d){
                    if (ok) {
                        try {
                            std::string block_id = d["head_block_id"].get<std::string>();
                            auto tx = PiChainUtility::Transfer2(sign_key, from_id, to_pub_key, amount, "1.3.0", block_id);
                            qDebug() << tx.dump(2).c_str();
                            this->BroadcastTransaction(tx, [cb](bool ok, const json &d) {
                                qDebug() << d.dump(2).c_str();
                                cb(ok);
                            });
                        } catch (...) {
                            cb(false);
                        }
                    } else {
                        cb(false);
                    }
                });
        } else {
            cb(false);
        }
    });
}

