#include "buildhistorydialog.h"
#include "ui_buildhistorydialog.h"

BuildHistoryDialog::BuildHistoryDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BuildHistoryDialog)
{
    ui->setupUi(this);
}

BuildHistoryDialog::~BuildHistoryDialog()
{
    delete ui;
}
