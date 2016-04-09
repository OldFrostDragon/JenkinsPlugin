#include "buildhistorydialog.h"
#include "ui_buildhistorydialog.h"

using namespace JenkinsPlugin::Internal;

BuildHistoryDialog::BuildHistoryDialog(JenkinsJob job, BuildHistoryModel *buildHistoryModel,
                                       QWidget *parent)
    : QDialog(parent), ui(new Ui::BuildHistoryDialog)
{
    ui->setupUi(this);
    _buildHistoryModel = buildHistoryModel;
    ui->listView->setModel(_buildHistoryModel);
    _buildHistoryModel->fetchBuildHistoryFor(job);
}

BuildHistoryDialog::~BuildHistoryDialog()
{
    delete ui;
    _buildHistoryModel->deleteLater();
}
