#ifndef BUILDHISTORYDIALOG_H
#define BUILDHISTORYDIALOG_H

#include <QDialog>

namespace Ui {
class BuildHistoryDialog;
}

class BuildHistoryDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BuildHistoryDialog(QWidget *parent = 0);
    ~BuildHistoryDialog();

private:
    Ui::BuildHistoryDialog *ui;
};

#endif // BUILDHISTORYDIALOG_H
