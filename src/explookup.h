#ifndef EXPLOOKUP_H
#define EXPLOOKUP_H
#include <QString>
#include <QAbstractTableModel>
#include <QList>
#include <QDateTime>

struct expEntry
{
    QString lookupKey;
    QString areaName;
    QString npcName;
    int exp;
    int kills;
};


class explookup : public QAbstractTableModel
{
    Q_OBJECT

signals:
    void expSignal(int totalExp, int secspassed);

public:
    explookup();
    explookup(QObject *parent);
    explookup(QString path);
    int rowCount(const QModelIndex &parent = QModelIndex()) const ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    void loadExpFile();
    void registerKill(QString areaName, QString npcName);
    void setMode(int mode);
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex & /*index*/) const;
    bool setData(const QModelIndex & index, const QVariant & value, int role);
    void clearCurrentKills();
    void saveLibrary();

private:
    void calculateExp();
    QDateTime lastClear;
    QList<expEntry> expList;
    QList<expEntry> currentKills;
    QList<expEntry> totalKills;
    QString path;
    int mode = 0;
};

#endif // EXPLOOKUP_H
