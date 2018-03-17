#ifndef PIWALLET_H
#define PIWALLET_H

#include <memory>
#include <vector>
#include <string>

class PiWallet
{
public:
    PiWallet();

    std::string CurrentPublicKey() const;
    std::string CurrentPivateKey() const;
    std::string CurrentAccountName() const;

    bool Load();
    bool Save();

    bool Import(const std::string &path);
    bool Export(const std::string &path);

    static std::shared_ptr<PiWallet> GetWallet();

    std::string name;
    std::string pri_key;
    std::string pub_key;

};

#endif // PIWALLET_H
