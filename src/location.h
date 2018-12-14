#ifndef LOCATION_H
#define LOCATION_H
#include <QString>
#include <QList>
#include <QTableWidget>
#include "config.h"



class Location
{
public:
    Location();
    Location(QString temp, QTableWidget *outputtemp, Config * appConfig);
    void parseLine(QString temp);
    QString getName();
    void setSortingEnabled(bool state);


private:
    Config * appConfig;
    QString name;
    QList<QString> lines;
    QTableWidget  * tblOutput;

    void updateLocationsStatsTable(QString Source, QString Target, QString Skill, int Damage,  bool UpdateDam,bool UpdateCount, bool UpdateAVG, bool SourceMatch, bool TargetMatch, bool SkillMatch);
};

#endif // LOCATION_H
