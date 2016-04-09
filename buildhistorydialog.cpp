#include "buildhistorydialog.h"
#include "ui_buildhistorydialog.h"

using namespace JenkinsPlugin::Internal;

BuildHistoryDialog::BuildHistoryDialog(JenkinsJob job, BuildHistoryModel *buildHistoryModel,
                                       QWidget *parent)
    : QDialog(parent), ui(new Ui::BuildHistoryDialog)
{
    ui->setupUi(this);
    _buildHistoryModel = buildHistoryModel;
    ui->treeView->setModel(_buildHistoryModel);
    ui->treeView->setHeaderHidden(false);
    ui->treeView->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->treeView->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    ui->treeView->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    ui->treeView->header()->setSectionResizeMode(3, QHeaderView::Stretch);
    _buildHistoryModel->fetchBuildHistoryFor(job);
    setWindowTitle(QString(QStringLiteral("Build history: %1")).arg(job.name()));
}

BuildHistoryDialog::~BuildHistoryDialog()
{
    delete ui;
    _buildHistoryModel->deleteLater();
}
