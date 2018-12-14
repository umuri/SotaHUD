#include "notificationmanager.h"

notificationManager::notificationManager()
{

}

void notificationManager::addNotification(QString tempTitle, QString tempDescription, int secondsTimeout)
{
    if ( disabled == true )
    {
        return;
    }
    notification * newNotification;
    newNotification = new notification;
    newNotification->showNotification(tempTitle, tempDescription, secondsTimeout);
    notificationList.append(newNotification);
    connect(notificationList[notificationList.length()-1],SIGNAL(closed()),this,SLOT(notificationClosed()));
    repositionAll();
}

void notificationManager::notificationClosed()
{
    for ( int i = 0; i < notificationList.length(); i++ )
    {
        if ( notificationList[i]->closing() == true)
        {
            notification * tempNotification;
            tempNotification = notificationList[i];
            notificationList.removeAt(i);
            tempNotification->deleteLater();
        }

    }
    repositionAll();
}


void notificationManager::closeAll()
{

    for ( int i = 0; i < notificationList.length(); i++ )
    {
            notification * tempNotification;
            tempNotification = notificationList[i];
            notificationList.removeAt(i);
            tempNotification->close();
            tempNotification->deleteLater();
    }

}
void notificationManager::repositionAll()
{
    int totalHeight = 0; //First notification needs no offset
    for ( int i = notificationList.length()-1; i >= 0; i-- )
    {
        notificationList[i]->moveToYOffset(totalHeight);
        totalHeight += notificationList[i]->height();
    }


}

void notificationManager::disableNotifications()
{
    disabled = true;
}

void notificationManager::enableNotifications()
{
    disabled = false;
}
