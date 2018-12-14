#include "notification.h"
#include "ui_notification.h"


notification::notification(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::notification)
{
    ui->setupUi(this);

    setAttribute(Qt::WA_ShowWithoutActivating);
    setWindowFlags(
                Qt::Window | // Add if popup doesn't show up
                Qt::ToolTip | //Avoids taskbar icon
                Qt::FramelessWindowHint | // No window border
                Qt::WindowDoesNotAcceptFocus | // No focus
                Qt::WindowStaysOnTopHint // Always on top
                );

    connect(ui->button, SIGNAL(clicked()), this, SLOT(closeNotification()));
}

notification::~notification()
{
    delete ui;
}

bool notification::closing()
{
    return isClosing;
}
void notification::timerEvent(QTimerEvent * /*e*/)
{
    closeNotification();
}

void notification::moveToYOffset(int offset)
{
    // resize the widget, as text label may be larger than the previous size
    QRect rect = QStyle::alignedRect(
                        Qt::RightToLeft,
                        Qt::AlignBottom,
                        size(),
                        qApp->desktop()->availableGeometry());
    rect.moveTop((rect.y()-offset));
    setGeometry(rect);

}

void notification::showNotification(QString title, QString description, int secondsTimeout)
{
    ui->title->setText(QString("<b>%1</b>").arg(title));
    ui->description->setText(description);
    startTimer(1000 * secondsTimeout);

    ui->description->adjustSize();
    adjustSize();
    // resize the widget, as text label may be larger than the previous size
    setGeometry(QStyle::alignedRect(
                    Qt::RightToLeft,
                    Qt::AlignBottom,
                    size(),
                    qApp->desktop()->availableGeometry()));

    show();
}


void notification::closeNotification()
{
    isClosing = true;
    emit closed();
    close();
}

void notification::mousePressEvent(QMouseEvent*)
{
    emit clicked();
}
