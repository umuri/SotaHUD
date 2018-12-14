#ifndef CONFIG_H
#define CONFIG_H
#include <QFile>
#include <QStandardPaths>
#include <QDateTime>

extern const int MajorVersion;
extern const int MinorVersion;
extern const int RevisionVersion;

class Config
{
public:
    bool ParsePastDay = false;
    long MaxDisplay = 10000;
    int MaxClearLength = 1000;
    bool ParseCurrentDay = false;
    bool ShowKillingBlowSkillUsed = false;
    bool ShowCriticalHitsSeperate = false;
    bool StartOnTop = false;
    bool SalesSound = false;
    bool SalesPopup = false;
    int SalesTime = 10;
    bool WhisperSound = false;
    bool WhisperPopup = false;
    int WhisperTime = 10;
    bool StartTrackPlayer = false;
    bool LogonLogoffPopup = false;
    bool LogonLogoffSound = false;
    int LogonLogoffTime = 10;
    QString LogDateTimeFormat = "";
    QString SystemLocaleName = "";
    QString FileDateFormat = "yyyy-MM-dd";
    bool loginNotificationListOn = false;
    QList<QString> loginNotificationList;

    int ShowEventsHappeningInXHours = 48;
    QString appDataPath;
    bool appDataPathChanged = false;
    int Volume = 50;
    QString CharacterName = "";
    QString DefaultSavePath = "\\";
    QDateTime pastDayStart;
    QDateTime pastDayEnd;
    Config();
    int loadConfig();

    void changeSettings();


    //FINDME: ADDWINDOW

    QString mainWindowSettings = "";
    QString mapWindowSettings = "";
    QString expWindowSettings = "";
    QString statsWindowSettings = "";
    QString clockWindowSettings = "";
    QString combatWindowSettings = "";
    QString dpsWindowSettings = "";
    QString questWindowSettings = "";

    void saveSettings();
};

#endif // CONFIG_H
