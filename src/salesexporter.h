#ifndef SALESEXPORTER_H
#define SALESEXPORTER_H
#include "notificationmanager.h"
#include "config.h"
#include <QClipboard>
#include <QTextStream>
#include <QFile>

class SalesExporter
{
public:
    explicit SalesExporter(Config * tempConfig, notificationManager * tempPopup);
    void CheckForSale(QString inputString);

private:
    notificationManager * popupManager;
    Config * appConfig;
    QFile * salesFile;
    QTextStream * salesOut;
    ~SalesExporter();
};

#endif // SALESEXPORTER_H
