#ifndef NOTIFICATIONMANAGER_H
#define NOTIFICATIONMANAGER_H

#include "notification.h"
#include <QList>
class notificationManager : public QObject
{
        Q_OBJECT

public:
    notificationManager();
    void addNotification(QString tempTitle, QString tempDescription, int secondsTimeout = 10);
    void closeAll();



    void disableNotifications();
    void enableNotifications();
private slots:
    void notificationClosed();

private:
    QList<notification *> notificationList;

    bool disabled = false;
    void repositionAll();
};

#endif // NOTIFICATIONMANAGER_H
