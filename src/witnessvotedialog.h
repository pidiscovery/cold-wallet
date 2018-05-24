#ifndef WITNESSVOTEDIALOG_H
#define WITNESSVOTEDIALOG_H

#include <QDialog>

namespace Ui {
class WitnessVoteDialog;
}

class WitnessVoteDialog : public QDialog
{
    Q_OBJECT

public:
    explicit WitnessVoteDialog(QWidget *parent = 0);
    ~WitnessVoteDialog();
    std::string GetWitness() const;

private slots:
    void on_pushButtonVote_clicked();

private:
    Ui::WitnessVoteDialog *ui;
    std::string witness_account;
};

#endif // WITNESSVOTEDIALOG_H
