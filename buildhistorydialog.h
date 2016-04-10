#ifndef BUILDHISTORYDIALOG_H
#define BUILDHISTORYDIALOG_H

#include <QDialog>

#include "buildhistorymodel.h"

namespace Ui
{
class BuildHistoryDialog;
}

namespace JenkinsPlugin
{
namespace Internal
{
class BuildHistoryDialog : public QDialog
{
    Q_OBJECT
public:
    explicit BuildHistoryDialog(JenkinsJob job, BuildHistoryModel *buildHistoryModel,
                                QWidget *parent = 0);
    ~BuildHistoryDialog();

private slots:
    void onBuildEntryDoubleClicked(QModelIndex index);

private:
    Ui::BuildHistoryDialog *ui;
    BuildHistoryModel *_buildHistoryModel{nullptr};
};
}
}

#endif  // BUILDHISTORYDIALOG_H
