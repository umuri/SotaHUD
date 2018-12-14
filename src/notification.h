#ifndef NOTIFICATION_H
#define NOTIFICATION_H
#include <QDialog>
#include <QDesktopWidget>
#include <QMouseEvent>

namespace Ui {
    class notification;
}

class notification : public QDialog
{
    Q_OBJECT

public:
    explicit notification(QWidget *parent = 0);
    ~notification();
    bool closing();

    void showNotification(QString title, QString description, int secondsTimeout = 10);
    void mousePressEvent(QMouseEvent*);
    void moveToYOffset(int offset);


private slots:
    void closeNotification();

signals:
    void closed();
    void clicked();

private:
    Ui::notification *ui;
    bool isClosing = false;

protected:
    void timerEvent(QTimerEvent *);
};
#endif // NOTIFICATION_H

