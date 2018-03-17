#ifndef PIWEBSOCKETCLIENT_H
#define PIWEBSOCKETCLIENT_H

#include <QObject>
#include <QtWebSockets/QWebSocket>
#include <map>
#include <nlohmann/json.hpp>
#include <functional>
#include <memory>
//#include <future>
#include <thread>


class PiWebsocketClient : public QObject
{
    Q_OBJECT
public:
    explicit PiWebsocketClient(QObject *parent = nullptr);

    void ApiRegister(const std::string &type, std::function<void (bool)> callback);

    void GetDynamiGlobalProperties(std::function<void (bool, const nlohmann::json&)> cb);
    void GetAccountIdByName(const std::string &name, std::function<void (bool, const std::string&)> cb);
    void GetAccountByName(const std::string &name, std::function<void (bool, const nlohmann::json&)> cb);
    void GetAccountHistory(const std::string &name, int stop, int num, int start, std::function<void (bool, const nlohmann::json&)> cb);
    void BroadcastTransaction(const nlohmann::json &tx, std::function<void(bool, const nlohmann::json&)> cb);

    void GetBalance(const std::string &name, std::function<void(bool, uint64_t)> cb);

    void GetAccountNameByPublicKey(const std::string &key, std::function<void(bool ok, const std::string&)> cb);

    void GetAccountNameById(const std::string &id, std::function<void(bool ok, const std::string&)> cb);
    bool IsReady();

    void TransferTo(const std::string sign_key, const std::string &from, const std::string &to, uint64_t amount, const std::string &asset, std::function<void(bool)> cb);

    void TransferTo2(const std::string sign_key, const std::string &from, const std::string &to_pub_key, uint64_t amount, const std::string &asset, std::function<void(bool)> cb);

signals:
    void login(bool ok);

public slots:

private:
    std::string BuildRequestJson(const std::string &method, int id, const nlohmann::json &params);
    int GetNextId();
    bool CallServer(int id, const std::string &data, std::function<void(bool, const nlohmann::json&)> cb);

    void Login(std::function<void(bool)> cb);
    int GetApiId(const std::string &api);

    std::shared_ptr<QWebSocket> websocket_client;
    bool is_connect;
    bool is_login;
    std::map<std::string, int> api_map;
    int inc_id;
    std::map<int, std::function<void(bool, const nlohmann::json&)> > cb_map;

    std::mutex mutex;
};

#endif // PIWEBSOCKETCLIENT_H
