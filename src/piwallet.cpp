#include "piwallet.h"
#include <QStandardPaths>
#include <nlohmann/json.hpp>
#include "pichainutility.h"
#include <QFile>
#include <fstream>
#include <QDir>

using json = nlohmann::json;

PiWallet::PiWallet()
{
    Load();
}


std::shared_ptr<PiWallet> PiWallet::GetWallet() {
    static std::shared_ptr<PiWallet> instance = std::make_shared<PiWallet>();
    return instance;
}

std::string PiWallet::CurrentPublicKey() const {
    return pub_key;
}

std::string PiWallet::CurrentPivateKey() const {
    return pri_key;
}

std::string PiWallet::CurrentAccountName() const {
    return name;
}

//std::string PiWallet::CurrentAccountId() const {
//    return "1.2.30273";
//}

bool PiWallet::Load() {
    QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (!QFile(dir + "/wallet.json").exists()) {
        auto acc = PiChainUtility::GenerateAccount();
        name = std::get<0>(acc);
        pub_key = std::get<1>(acc);
        pri_key = std::get<2>(acc);
        Save();
        return true;
    }
    std::ifstream ifs(dir.toStdString() + "/wallet.json");
    json j;
    ifs >> j;
    name = j["name"].get<std::string>();
    pub_key = j["public"].get<std::string>();
    pri_key = j["private"].get<std::string>();
    return true;
}

bool PiWallet::Save() {
    QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (!QDir(dir).exists()) {
        QDir().mkdir(dir);
    }
    json j;
    j["name"] = name;
    j["public"] = pub_key;
    j["private"] = pri_key;

    std::ofstream ofs(dir.toStdString() + "/wallet.json");
    ofs << j;
    return true;
}

bool PiWallet::Import(const std::string &path) {
    std::ifstream ifs(path);
    json j;
    ifs >> j;
    name = j["name"].get<std::string>();
    pub_key = j["public"].get<std::string>();
    pri_key = j["private"].get<std::string>();
    Save();
    return true;
}

bool PiWallet::Export(const std::string &path) {
    json j;
    j["name"] = name;
    j["public"] = pub_key;
    j["private"] = pri_key;
    std::ofstream ofs(path);
    ofs << j;
    return true;
}

