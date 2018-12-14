#ifndef QUESTPLUSCONTROLLER_H
#define QUESTPLUSCONTROLLER_H

#include <QObject>
#include <questsplusquest.h>
#include "config.h"
#include "hudquestjournal.h"


class questsPlusController
{
public:
    questsPlusController();
    questsPlusController(Config *configTemp);
    int count();
    QString questNameAt(int index);
    questsPlusQuest * questAt(int index);
    QString questStatusAt(int index);
    int rowAt(questsPlusQuest *targetQuest);
    Config * appConfig;
    hudQuestJournal currentJournal;

    void registerCurrentLocation(QString unityName, float x, float y, float z);
    void registerKill(QString target);

    void saveQuests();
    void registerChatLog(QStringList target, ChatType::ChatType type);

    void registerLoot(QString item, int amount);
private:
    QList<questsPlusQuest> questList;

    void loadQuests();




};

#endif // QUESTPLUSCONTROLLER_H
