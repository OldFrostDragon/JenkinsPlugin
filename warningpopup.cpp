#include "warningpopup.h"
#include "ui_warningpopup.h"

WarningPopup::WarningPopup(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WarningPopup)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Window |                    // Add if popup doesn't show up
                   Qt::FramelessWindowHint |       // No window border
                   Qt::WindowDoesNotAcceptFocus |  // No focus
                   Qt::WindowStaysOnTopHint        // Always on top
                   );
//    setAttribute(Qt::WA_ShowWithoutActivating);
        setAttribute(Qt::WA_TranslucentBackground);
//    setAttribute(Qt::WA_TransparentForMouseEvents);
    connect(ui->closeButton, &QPushButton::clicked, this, &WarningPopup::close);
}

WarningPopup::~WarningPopup()
{
    delete ui;
}
