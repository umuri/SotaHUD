#ifndef HUDCLOCK_H
#define HUDCLOCK_H

#include <QWidget>
#include "hudwindow.h"
#include <QLocale>
#include <QDateTime>
#include <QLabel>
#include <QButtonGroup>
#include <QDebug>
#include <QInputDialog>
#include "config.h"
#include <QSoundEffect>
#include <notificationmanager.h>



namespace Ui {
class hudClock;
}
struct Event {
    QString eventName = "Unnamed Event";
    QString eventDescription = "No Description";
    QDateTime eventTime;
    int eventTimeZone = 0;
    int timerSecs = -1;
    int buttonID;
    bool fiveMinuteWarning = true;
    bool fifteenMinuteWarning = true;
    bool thirtyMinuteWarning = true;
    bool soundAlarm = true;
};

class hudClock : public hudWindow
{
    Q_OBJECT

public:
    explicit hudClock(QWidget *parent = 0, Config * tempConfig = NULL, notificationManager * tempPopup = NULL);
    ~hudClock();
    void setVolume(float volume);
    void addEvent(Event newEvent);
    void addTimer(QString timerText, int secs);


private:
    int buttonCount = 0;
    Ui::hudClock *ui;
    int timerID;
    int timeZoneOffset;
    QLocale systemLocale;
    QList<Event> eventList;
    QList<QLabel *> eventLabelList;
    QButtonGroup plusButtonGroup;
    QButtonGroup removeButtonGroup;
    QButtonGroup infoButtonGroup;
    QString lastTimerText;
    QString lastTimerTime;
    Config * appConfig;
    QSoundEffect * alarmSound;

    notificationManager * popupManager;

    void loadEvents();
    void processEventsFile(QString File);


protected:
    void timerEvent(QTimerEvent *e);


private slots:
    void removeButtonGroupClicked(int eventID);
    void addTimerActionClicked();
    void plusButtonGroupClicked(int eventID);
    void infoButtonGroupClicked(int eventID);
};

#endif // HUDCLOCK_H
