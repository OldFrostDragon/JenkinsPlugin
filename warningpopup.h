#ifndef WARNINGPOPUP_H
#define WARNINGPOPUP_H

#include <QWidget>

namespace Ui {
class WarningPopup;
}

class WarningPopup : public QWidget
{
    Q_OBJECT

public:
    explicit WarningPopup(QWidget *parent = 0);
    ~WarningPopup();

private:
    Ui::WarningPopup *ui;
};

#endif // WARNINGPOPUP_H
