#include "pihomewidget.h"
#include "ui_pihomewidget.h"
#include <QDebug>
#include <nlohmann/json.hpp>
#include "pichainutility.h"
#include <future>
#include <piwallet.h>
#include <QMessageBox>
#include <QClipboard>
#include <QStandardPaths>
#include <QFileDialog>

using json = nlohmann::json;

PiHomeWidget::PiHomeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PiHomeWidget),
    client(std::make_shared<PiWebsocketClient>(this))
{
    ui->setupUi(this);
    SwitchAccount();

    connect(client.get(), &PiWebsocketClient::login, [this](bool ok){
        // register api when login ready
        if (ok) {
            this->client->ApiRegister("database", [this](bool ok){
                if (ok) {
                    qDebug() << "database register done";
                    if (this->client->IsReady()) {
                        this->UpdateBalance();
                    }
                } else {
                    qDebug() << "database register fail";
                }
            });
            this->client->ApiRegister("history", [](bool ok){
                if (ok) {
                    qDebug() << "history register done";
                } else {
                    qDebug() << "history register fail";
                }
            });
            this->client->ApiRegister("network_broadcast", [](bool ok){
                if (ok) {
                    qDebug() << "network_broadcast register done";
                } else {
                    qDebug() << "network_broadcast register fail";
                }
            });
        }
    });
}

PiHomeWidget::~PiHomeWidget()
{
    delete ui;
}

void PiHomeWidget::SwitchAccount() {
    ui->labelAddressValue->setText(PiWallet::GetWallet()->CurrentPublicKey().c_str());
}

void PiHomeWidget::UpdateBalance() {
    auto wallet = PiWallet::GetWallet();
    client->GetBalance(wallet->CurrentAccountName(), [&](bool ok, uint64_t amount){
        ui->labelBalancePIC->setText(QString::number(amount / 100000.f));
    });
}

void PiHomeWidget::on_buttonExport_clicked()
{
    QString def_path = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString path = QFileDialog::getSaveFileName(this, "Save wallet to file", def_path + "/wallet.json", "account file (*.json)");
    if (path.isNull()) {
        return;
    }
    PiWallet::GetWallet()->Export(path.toStdString());
}

void PiHomeWidget::on_buttonImport_clicked()
{
    if (QMessageBox::Ok != QMessageBox::warning(this, "warning", "This operation will overwrite your current address and keys, go on import?"), QMessageBox::Ok) {
        return;
    }
    QString def_path = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString path = QFileDialog::getOpenFileName(this, "Load wallet from file", def_path, "wallet file (*.json)");
    if (path.isNull()) {
        return;
    }
    PiWallet::GetWallet()->Import("/Users/carl/Desktop/wallet.json");
    SwitchAccount();
    UpdateBalance();
}

void PiHomeWidget::on_buttonAbout_clicked()
{
    QMessageBox::about(this, "About", "π cold wallet.\nv0.1.0\n\n\n© pidiscovery\n2016-2018");
}

void PiHomeWidget::on_buttonCopyAddress_clicked()
{
    QString address = ui->labelAddressValue->text();
    QApplication::clipboard()->setText(address);
}

void PiHomeWidget::on_buttonTransfer_clicked()
{
    uint64_t amount = ui->doubleSpinBoxAmount->value() * 100000;
    if (amount <= 0) {
        QMessageBox::warning(this, "warning", "Sending amount cannot less than 0.");
        return;
    }

    std::string to_account = ui->lineEditToAddress->text().toStdString();
    if (to_account == "") {
        QMessageBox::warning(this, "warning", "To address cannot be empty.");
        return;
    }

    ui->lineEditToAddress->setEnabled(false);
    ui->lineEditToAddress->setEnabled(false);

    // check if to is public key
    if (PiChainUtility::IsPublicKey(to_account)) {
        // check if is registered
        client->GetAccountNameByPublicKey(to_account, [this, amount, to_account](bool ok, const std::string &name){
            if (ok) {
                auto wallet = PiWallet::GetWallet();
                qDebug() << "got name:" << name.c_str();
                this->client->TransferTo(wallet->CurrentPivateKey(), wallet->CurrentAccountName(), name, amount, "1.3.0", [this](bool ok){
                    this->ui->lineEditToAddress->setEnabled(true);
                    this->ui->lineEditToAddress->setEnabled(true);
                    if (ok) {
                        this->UpdateBalance();
                        QMessageBox::information(this, "Information", "Transfer done.");
                    } else {
                        QMessageBox::warning(this, "Warning", "Transfer Fail.");
                    }
                });
            } else {
                auto wallet = PiWallet::GetWallet();
                this->client->TransferTo2(wallet->CurrentPivateKey(), wallet->CurrentAccountName(), to_account, amount, "1.3.0", [this](bool ok){
                    this->ui->lineEditToAddress->setEnabled(true);
                    this->ui->lineEditToAddress->setEnabled(true);
                    if (ok) {
                        this->UpdateBalance();
                        QMessageBox::information(this, "Information", "Transfer done.");
                    } else {
                        QMessageBox::warning(this, "Warning", "Transfer Fail.");
                    }
                });
            }
        });
    } else {
        client->GetAccountIdByName(to_account, [this, to_account, amount](bool ok, const std::string &id){
            if (ok) {
                auto wallet = PiWallet::GetWallet();
                this->client->TransferTo(wallet->CurrentPivateKey(), wallet->CurrentAccountName(), to_account, amount, "1.3.0", [this](bool ok){
                    this->ui->lineEditToAddress->setEnabled(true);
                    this->ui->lineEditToAddress->setEnabled(true);
                    if (ok) {
                        this->UpdateBalance();
                        QMessageBox::information(this, "Information", "Transfer done.");
                    } else {
                        QMessageBox::warning(this, "Warning", "Transfer Fail.");
                    }
                });
            } else {
                QMessageBox::warning(this, "Warning", "account not exist");
                this->ui->lineEditToAddress->setEnabled(true);
                this->ui->lineEditToAddress->setEnabled(true);
            }
        });
    }
}

void PiHomeWidget::on_buttonRefreshBalance_clicked()
{
    UpdateBalance();
}
