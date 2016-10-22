#ifndef WARNINGPOPUP_H
#define WARNINGPOPUP_H

#include <QTimer>
#include <QDialog>

namespace Ui {
class WarningPopup;
}

class WarningPopup : public QWidget
{
    Q_OBJECT

public:
    explicit WarningPopup(QWidget *parent = 0);
    ~WarningPopup();

    // period is in miliseconds
    int showPeriod() const;
    void setShowPeriod(int showPeriod);

public slots:
    void showPopup(const QString &text);

private:
    Ui::WarningPopup *ui;

    int _showPeriod{30000}; // 30 seconds
    QString _text;
    QTimer _timer;
};

#endif // WARNINGPOPUP_H
