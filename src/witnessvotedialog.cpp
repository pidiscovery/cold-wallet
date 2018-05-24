#include "witnessvotedialog.h"
#include "ui_witnessvotedialog.h"
#include <QMessageBox>

WitnessVoteDialog::WitnessVoteDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WitnessVoteDialog),
    witness_account("")
{
    ui->setupUi(this);
}

WitnessVoteDialog::~WitnessVoteDialog()
{
    delete ui;
}

void WitnessVoteDialog::on_pushButtonVote_clicked()
{
    if (ui->lineEditWitness->text().toStdString() == "") {
        QMessageBox::warning(this, "warning", "witness should not be empty!");
        return;
    }
    witness_account = ui->lineEditWitness->text().toStdString();
    close();
}

std::string WitnessVoteDialog::GetWitness() const {
    return witness_account;
}
