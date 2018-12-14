#ifndef HUDCOMBAT_H
#define HUDCOMBAT_H

#include <QWidget>
#include <hudwindow.h>
#include <location.h>
#include <QTableWidget>
#include <QToolButton>
#include "config.h"
#include <QDebug>
namespace Ui {
class hudCombat;
}

class hudCombat : public hudWindow
{
    Q_OBJECT

public:
    explicit hudCombat(QWidget *parent = 0, Config * tempConfig = NULL);
    ~hudCombat();

    void enterLocation(QString location);
    void parseLine(QString line);
    void enableUpdates();
    void disableUpdates();
    QString getCurrentLocationName();



private slots:
    void newLocationButton();

private:
    QToolButton * tb;
    int newTabCount;
    Ui::hudCombat *ui;
    Config * appConfig;
    QList<Location *> Locations;
    Location * currentLocation;
    Location * defaultLocation;
};

#endif // HUDCOMBAT_H
