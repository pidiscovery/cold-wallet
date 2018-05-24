#ifndef PIHOMEWIDGET_H
#define PIHOMEWIDGET_H

#include <QWidget>
#include "piwebsocketclient.h"
#include <memory>

namespace Ui {
class PiHomeWidget;
}

class PiHomeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PiHomeWidget(QWidget *parent = 0);
    ~PiHomeWidget();

signals:

private slots:
    void on_buttonExport_clicked();

    void on_buttonImport_clicked();

    void on_buttonAbout_clicked();

    void on_buttonCopyAddress_clicked();

    void on_buttonTransfer_clicked();


    void on_buttonRefreshBalance_clicked();

    void on_commandLinkButtonVote_clicked();

private:
    Ui::PiHomeWidget *ui;
    std::shared_ptr<PiWebsocketClient> client;

    void SwitchAccount();
    void UpdateBalance();
};

#endif // PIHOMEWIDGET_H
