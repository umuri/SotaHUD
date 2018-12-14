#include "explookup.h"
#include <QDir>
#include <QTextStream>
#include <QDebug>

bool operator==(const expEntry& lhs, const expEntry& rhs)
{
    if  ( lhs.lookupKey == rhs.lookupKey)
        return true;
    else
        return false;
}


explookup::explookup()
{
    path = QDir::currentPath() + "/expList.umu";
    loadExpFile();
    clearCurrentKills();

}

explookup::explookup(QObject *parent)
  :QAbstractTableModel(parent)
{
    path = QDir::currentPath() + "/expList.umu";
    loadExpFile();
    clearCurrentKills();
}

explookup::explookup(QString pathTemp)
{
    path = pathTemp;
    loadExpFile();
    clearCurrentKills();
}

void explookup::loadExpFile()
{

    expList.clear();
    currentKills.clear();
    totalKills.clear();


    QFile expFile(path);
    if ( expFile.exists() )
    {
        expFile.open(QIODevice::ReadOnly);

        QTextStream expIn(&expFile);
        QString areaName;
        QString npcName;
        int expAmount;
        while(!expIn.atEnd())
        {
            QString line = expIn.readLine();

           QRegExp rx("([^.]*)\\.([^|]*)\\|([0-9]*)", Qt::CaseSensitive);
           if ( rx.indexIn(line,0) >= 0 )
           {
             areaName=rx.cap(1);
             npcName=rx.cap(2);
             expAmount=rx.cap(3).toInt();
             //qDebug() << rx.cap(1) << ":" << rx.cap(2) << ":" << rx.cap(3);
             //qDebug() << areaName << ":" << npcName << ":" << expAmount;


             expEntry tempEntry;
             tempEntry.areaName = areaName;
             tempEntry.kills = 0;
             tempEntry.exp = expAmount;
             tempEntry.lookupKey = areaName + "." + npcName;
             tempEntry.npcName = npcName;
             expList.append(tempEntry);
           }






        }
    }

}

void explookup::registerKill(QString areaName, QString npcName)
{
    expEntry key;
    key.lookupKey = areaName + "." + npcName;

    if ( expList.indexOf(key) < 0 ) //This isn't in our library, add it
    {
        expEntry tempEntry;
        tempEntry.lookupKey = key.lookupKey;
        tempEntry.areaName = areaName;
        tempEntry.npcName = npcName;
        tempEntry.exp = 0;
        tempEntry.kills = 0;

        if ( mode == 2 )
        {
            beginInsertRows(QModelIndex(), expList.count(), expList.count());
            expList.append(tempEntry);
            endInsertRows();
        }
        else
        {
            expList.append(tempEntry);
        }
    }

    if ( currentKills.indexOf(key) < 0 ) //If it's not in our list, add it
    {
        if ( mode == 1 )
        {
            beginInsertRows(QModelIndex(), currentKills.count(), currentKills.count());
            currentKills.append(expList[expList.indexOf(key)]);
            endInsertRows();
        }
        else
        {
            currentKills.append(expList[expList.indexOf(key)]);
        }
    }

    currentKills[currentKills.indexOf(key)].kills++; //Add a kill

    if ( totalKills.indexOf(key) < 0 ) //If it's not in our list, add it
    {
        if ( mode == 0 )
        {
            beginInsertRows(QModelIndex(), currentKills.count(), currentKills.count());
            totalKills.append(expList[expList.indexOf(key)]);
            endInsertRows();
        }
        else
        {
            totalKills.append(expList[expList.indexOf(key)]);
        }

    }

    totalKills[totalKills.indexOf(key)].kills++; //Add a kill

    int row;
    switch (mode)
    {
    case 0 :
        row = totalKills.indexOf(key);
        break;
    case 1 :
        row = currentKills.indexOf(key);
        break;
    case 2 :
    default:
        row = expList.indexOf(key);
        break;
    }
    dataChanged(index(row,0),index(row,columnCount()-1));
    calculateExp();
}

void explookup::clearCurrentKills()
{
    if ( mode == 1 )
    {
     beginResetModel();
    }
    currentKills.clear();
    if ( mode == 1 )
    {
     endResetModel();
    }

    lastClear = QDateTime::currentDateTime();
    calculateExp();
}

void explookup::calculateExp()
{

    int totalExp = 0;
    const QList<expEntry> * target;
    switch(mode)
    {
        case 0:
            target = &totalKills;
            break;
        case 1:
            target = &currentKills;
            break;
        case 2:
        default:
            target = &expList;
            break;
    }
    if ( target->length() == 0 )
    {
        emit expSignal(0,0);
        return;
    }
    for ( int i = 0; i < target->length(); i++)
    {
        totalExp += target->at(i).exp * target->at(i).kills;
    }

    emit expSignal(totalExp, lastClear.secsTo(QDateTime::currentDateTime()));

}


int explookup::rowCount(const QModelIndex & /*parent*/) const
{
    //Mode 0 is all kills
    //Mode 1 is current kills
    //Mode 2 is library
    switch(mode)
    {
        case 0 : return totalKills.length();
        case 1 : return currentKills.length();
        case 2 : return expList.length();
    }
   return 0;
}

int explookup::columnCount(const QModelIndex & /*parent*/) const
{
    return 5;
}

QVariant explookup::data(const QModelIndex &index, int role) const
{

    if (role == Qt::DisplayRole)
       {


    const QList<expEntry> * target;
    switch(mode)
    {
        case 0:
            target = &totalKills;
            break;
        case 1:
            target = &currentKills;
            break;
        case 2:
        default:
            target = &expList;
            break;
    }
    switch(index.column())
    {
    case 0:
        return target->at(index.row()).areaName;
        break;
    case 1:
        return target->at(index.row()).npcName;
        break;
    case 2:
        return target->at(index.row()).exp;
        break;
    case 3:
        return target->at(index.row()).kills;
        break;
    case 4:
    default:
        return target->at(index.row()).kills * target->at(index.row()).exp;
        break;
    }
    }
    return QVariant();
}

void explookup::setMode(int modeTemp)
{
    mode = modeTemp;

    QModelIndex topLeft = index(0, 0);
    QModelIndex bottomRight = index(rowCount() - 1, columnCount() - 1);

    calculateExp();
    emit dataChanged(topLeft, bottomRight);
    emit layoutChanged();
}


QVariant explookup::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal) {
            switch (section)
            {
            case 0:
                return QString("Area");
            case 1:
                return QString("NPC Name");
            case 2:
                return QString("ExpPerKill");
            case 3:
                return QString("Kills");
            case 4:
                return QString("Total");
            }
        }
    }
    return QVariant();
}

Qt::ItemFlags explookup::flags(const QModelIndex & index) const
{
    if ( index.column() == 2)
    {
        return Qt::ItemIsSelectable |  Qt::ItemIsEditable | Qt::ItemIsEnabled ;
    }
        else return Qt::ItemIsEnabled;
}



bool explookup::setData(const QModelIndex & index, const QVariant & value, int role)
{
    if (( index.column() == 2 ) && (role == Qt::EditRole) )
    {
        int newExpValue;
        //save value from editor to member m_gridData
        newExpValue = value.toInt();

        expEntry tempEntry;
        const QList<expEntry> * target;
        switch(mode)
        {
            case 0:
                target = &totalKills;
                break;
            case 1:
                target = &currentKills;
                break;
            case 2:
            default:
                target = &expList;
                break;
        }

        expEntry tempKey;
        tempKey.lookupKey = target->at(index.row()).lookupKey;

        totalKills[totalKills.indexOf(tempKey)].exp = newExpValue;
        currentKills[currentKills.indexOf(tempKey)].exp = newExpValue;
        expList[expList.indexOf(tempKey)].exp = newExpValue;

        calculateExp();

    }
    return true;
}

void explookup::saveLibrary()
{


    QString savePath = QDir::currentPath() + "/expList.umu";
    QFile saveFile(savePath);

    saveFile.open(QIODevice::WriteOnly|QIODevice::Truncate);


    QTextStream saveOut(&saveFile);

    for ( int i = 0; i < expList.length(); i++ )
    {
        saveOut << expList[i].areaName << "." << expList[i].npcName << "|" << expList[i].exp << endl;
    }
    saveOut.flush();
    saveFile.close();
}
