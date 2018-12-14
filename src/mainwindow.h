#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "location.h"
#include "config.h"
#include <QMainWindow>
#include <QTextStream>
#include <QTimer>
#include <QFile>
#include <QTableWidget>
#include <QToolButton>
#include <QQueue>
#include <QListWidget>
#include <QProgressDialog>
#include <QSoundEffect>
#include "map.h"
#include "notification.h"
#include "hudexp.h"
#include "hudmap.h"
#include "hudstats.h"
#include "hudclock.h"
#include "huddecogrid.h"
#include "hudcombat.h"
#include "huddps.h"
#include "notificationmanager.h"
#include "salesexporter.h"
#include "questspluscontroller.h"
#include "hudquest.h"

extern const int MajorVersion;
extern const int MinorVersion;
extern const int RevisionVersion;



namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT



public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    Config appConfig;
    notificationManager popupManager;
    bool DEBUG = false;
    QString AppDataPathPrefix;


private:
    QQueue<QString> readBuffer;
    QList<Location *> Locations;
    Ui::MainWindow *ui;
    QFile * myFile;
    QTextStream * in;
    QString characterName;
    QString currentPlayerDataPath;
    QAction * framelessAct;
    QString currentUnityZone = "";
    void updateText();
     int timerID;
    void parseLine(QString temp);
    void parseStatsCommand(QString temp, QDateTime qdttemp);
    void sortStatsCommand(QString tempKey, QString tempValue);
    void insertIntoTable(QTableWidget * table, QString tempKey, QString tempValue);
    void contextMenuEvent(QContextMenuEvent *event);
    QString statusBarStyleTemp;
    QString mainTabStyleTemp;
    bool bulkLoading = false;

    void parseSalvage(QString temp);
    void parseCrafting(QString temp);
    void parseFishing(QString temp);
    int threadRest;
    const int maxThreadRest = 10000;
    void addTextToTxt(QListWidget * windowToAdd, QString text);
    QProgressDialog * progress;
    QDateTime lastRead;

    hudQuest * questWindow;
    hudMap * mapWindow;
    hudExp * expWindow;
    hudStats * statsWindow;
    hudClock * clockWindow;
    hudDecoGrid * decoWindow;
    hudCombat * combatWindow;
    hudDPS * dpsWindow;
    SalesExporter * salesChecker;
    questsPlusController * questsController;

    QTimer * controlPointTimer;
    QTimer * mainTrackPlayerTimer;
    int controlPointWave;
    int controlPointTime;
    QSoundEffect waveSound;
    QSoundEffect warning1Sound;
    QSoundEffect warning2Sound;
    QSoundEffect saleSound;
    QSoundEffect messageSound;
    QSoundEffect logonSound;
    QSoundEffect logoffSound;
    void setVolumeUniversal(int volume);


    QRegExp HarvestRegExp;
    QRegExp LootRegExp;


    //Mouse window move code
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    int m_nMouseClick_X_Coordinate;
    int m_nMouseClick_Y_Coordinate;

    void disableUpdateEvents();
    void enableUpdateEvents();


    void setWindowClickThrough();

    void closeEvent(QCloseEvent *event);

    void saveWindowSettings();

    void loadWindowSettings();
    QString getWindowSettings();

    void setWindowSettings(QString settings);

protected:
       bool eventFilter( QObject *o, QEvent *e );



private slots:
    void changeTabViaContext(QAction * action);

    void toggleOnTop(bool state);
    void toggleFrameless(bool state);
    void toggleFramelessCheck();
    void setOpacityValue(int value);
    void exportCurrentVisibleAsCSV();
    void showAboutDialog();
    void showConfigDialog();

    void on_btnControlPoint_clicked();
    void controlPointTimerEvent();
    void mainTrackPlayerTimerEvent();

    void CheckForSaleToPass();



    void on_btnMapWindow_clicked();

    void setMapOpacityValue(int newValue);


    void on_chkMapClickthrough_toggled(bool checked);


    void on_btnExpWindow_clicked();


    void setExpOpacityValue(int newValue);


    void on_chkExpClickthrough_toggled(bool checked);

    void on_btnStatsWindow_clicked();
    void setStatsOpacityValue(int newValue);
    void on_chkStatsClickthrough_toggled(bool checked);

    void on_btnClockWindow_clicked();

    void setClockOpacityValue(int newValue);
    void on_chkClockClickthrough_toggled(bool);
    void on_btnDeco_clicked();

    void setCombatOpacityValue(int newValue);


    void on_btnCombatWindow_clicked();


    void on_chkCombatClickthrough_toggled(bool);
    void showCreditsDialog();


    void changeEvent(QEvent *e);
    void setDPSOpacityValue(int newValue);
    void on_btnDPSWindow_clicked();
    void on_chkDPSClickthrough_toggled(bool);

    void on_btnQuestWindow_clicked();

    void setQuestOpacityValue(int newValue);
    void on_chkQuestClickthrough_toggled(bool checked);

protected:
    void timerEvent(QTimerEvent *e);

};

#endif // MAINWINDOW_H

