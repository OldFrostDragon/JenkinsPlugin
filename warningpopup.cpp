#include "warningpopup.h"
#include "ui_warningpopup.h"

#include <QApplication>
#include <QDesktopWidget>

WarningPopup::WarningPopup(QWidget *parent) : QWidget(parent), ui(new Ui::WarningPopup)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Window |                    // Add if popup doesn't show up
                   Qt::FramelessWindowHint |       // No window border
                   Qt::WindowDoesNotAcceptFocus |  // No focus
                   Qt::WindowStaysOnTopHint        // Always on top
                   );
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_ShowWithoutActivating);
    setAttribute(Qt::WA_DeleteOnClose);

    _timer.setSingleShot(true);
    connect(ui->closeButton, &QPushButton::clicked, this, &WarningPopup::close);
    connect(&_timer, &QTimer::timeout, this, &WarningPopup::close);

    hide();
}

WarningPopup::~WarningPopup() { delete ui; }

int WarningPopup::showPeriod() const { return _showPeriod; }

void WarningPopup::setShowPeriod(int showPeriod) { _showPeriod = showPeriod; }

void WarningPopup::showPopup(const QString &text)
{
    auto activeWindow = qApp->activeWindow();
    QRect popupRect;
    if(activeWindow == nullptr)
    {
        popupRect = QStyle::alignedRect(Qt::RightToLeft, Qt::AlignBottom, size(),
                                              qApp->desktop()->screenGeometry(QCursor::pos()));
        popupRect.translate(-20, -30);
    }
    else
    {
        popupRect = QStyle::alignedRect(Qt::RightToLeft, Qt::AlignBottom, size(),
                                              activeWindow->geometry());
        // do not overlap with scrollbar and bottom panel of main Qt Creator window
        popupRect.translate(-20, -43);
    }
    setGeometry(popupRect);

    ui->warningTextlabel->setText(text);
    _timer.setInterval(_showPeriod);
    show();
    _timer.start();
}
