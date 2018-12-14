#ifndef HUDSTATS_H
#define HUDSTATS_H

#include <QWidget>
#include "hudwindow.h"
#include <QTableWidget>
#include <QDateTime>
#include <QDebug>
#include <QMap>
#include <QHash>

namespace Ui {
class hudStats;
}

class hudStats : public hudWindow
{
    Q_OBJECT





public:
    explicit hudStats(QWidget *parent = 0);
    ~hudStats();  
    void parseStatsCommand(QString temp, QDateTime);
    int getAdvLevel();
    int getProdLevel();
    QString getStatistic(QString stats);

private:

    Ui::hudStats *ui;

    QHash<QString,QString> currentStats;

    void insertIntoTable(QTableWidget *table, QString tempKey, QString tempValue);
    void sortStatsCommand(QString tempKey, QString tempValue);
    void processOverview();
};

#endif // HUDSTATS_H
