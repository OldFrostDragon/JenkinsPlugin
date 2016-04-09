#include "buildhistorydialog.h"
#include "ui_buildhistorydialog.h"

using namespace JenkinsPlugin::Internal;

BuildHistoryDialog::BuildHistoryDialog(JenkinsJob job, std::shared_ptr< RestRequestBuilder > builder,
                                       QWidget *parent)
    : QDialog(parent), ui(new Ui::BuildHistoryDialog)
{
    ui->setupUi(this);
    _buildHistoryModel = new BuildHistoryModel(builder);
    ui->listView->setModel(_buildHistoryModel);
    _buildHistoryModel->fetchBuildHistoryFor(job);
}

BuildHistoryDialog::~BuildHistoryDialog()
{
    delete ui;
    _buildHistoryModel->deleteLater();
}
