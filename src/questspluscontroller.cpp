#include "questspluscontroller.h"
#include <QDir>
#include <QFile>
#include <QDebug>


questsPlusController::questsPlusController(Config * configTemp)
{
    appConfig = configTemp;
    //Instantiate our Quests List internally
    questList.clear();

    //Load in our Quests List file and parse quests into objects
    loadQuests();

}
void questsPlusController::saveQuests()
{
    //qDebug() << "Saving quests?";
    for ( int i = 0; i < questList.count(); ++i )
    {
        //qDebug() << "Saving quest " << i;
        questList[i].saveQuest();
    }

    questList.clear();

}

int questsPlusController::count()
{
    return questList.count();
}

QString questsPlusController::questNameAt(int index)
{
    return questList[index].getQuestName();

}

int questsPlusController::rowAt(questsPlusQuest * targetQuest)
{
    for ( int i = 0; i < questList.count(); i++ )
    {
        if ( &questList[i] == targetQuest )
        {
            return i;
        }
    }
    return -1;
}

questsPlusQuest * questsPlusController::questAt(int index)
{
    if ( ( index >= 0 ) && ( index < questList.count() ) )
    {
        return &questList[index];
    }
    return NULL;
}

QString questsPlusController::questStatusAt(int index)
{
    return questList[index].getQuestStatus();
}

void questsPlusController::loadQuests() {

    QString pathTemp;
    //Clear list in case we are reloading
    questList.clear();

    //Load quest list folder, should be under /quests/
    pathTemp = QDir::currentPath() + "/quests/";

    QDir questFolder(pathTemp);

    //Show only subdirectories
    questFolder.setFilter(QDir::Dirs);


    if ( !QDir(QDir::currentPath() + "/quests").exists() )
    {
        QDir().mkdir(QDir::currentPath() + "/quests");
    }

    QString questDebugPath = QDir::currentPath() + "/quests/questDebug.txt";
    QFile questDebugFile(questDebugPath);

    questDebugFile.open(QIODevice::WriteOnly|QIODevice::Truncate);

    QTextStream questDebugOut(&questDebugFile);


    //Iterate through list of files and folders
    for ( unsigned int i = 0; i < questFolder.count(); i++)
    {

        //Exclude the current directory and previous directory symbolic link
        if (( questFolder.entryList()[i] != ".") && (questFolder.entryList()[i] != ".."))
        {
            //Assign to variable to prevent multiple lookups
            QString newQuestFolder = questFolder.entryList()[i];

            //We found a quest folder! Make a new quest object
            questsPlusQuest * newQuest = new questsPlusQuest(newQuestFolder);

            //Is this valid, any parse errors, whatsoever?
            if ( newQuest->isValid() )
            {
                questDebugOut << "Valid quest found: " << newQuestFolder << endl;
                questList.append(*newQuest);
            }
            else
            {
                questDebugOut << "Invalid quest found: " << newQuestFolder << endl;
                questDebugOut << "    Error: " << newQuest-> getError() << endl;
            }

            //qDebug() << questFolder.entryList()[i] << endl;
        }

    }



    for ( int i = 0; i < questList.count(); i++ )
    {
        //qDebug() << questList[i].getQuestName() << " " << &questList[i];
        for ( int j = 0; j < questList[i].getGoalsCount(); j++)
        {
            //qDebug() << "QuestController: " << questList[i].getGoal(j) << " " << questList[i].getGoalPtr(j) << " " << questList[i].getGoalPtr(j)->parent;
        }
    }


    questDebugOut.flush();
    questDebugFile.close();
}


void questsPlusController::registerCurrentLocation(QString unityName,float x,float y,float z)
{
    for ( int i = 0; i < questList.count(); i++)
    {
        questList[i].tickTimers();
        //qDebug() << "Quest " << i << ": " << questList[i].isCompleted() << "    " << questList[i].hasPosTRIGGER();
        if ( ( questList[i].isCompleted() == false ) && ( questList[i].hasPosTRIGGER() == true ))
        {

            questList[i].registerCurrentLocation(unityName,x,y,z);
        }
    }

}

void questsPlusController::registerKill(QString target)
{
    //qDebug() << "Kill attempt: " << target;
    for ( int i = 0; i < questList.count(); i++)
    {
        if ( ( questList[i].isCompleted() == false ) && ( questList[i].hasKillTRIGGER() == true ))
        {
            //qDebug() << "Kill attempt quest hit: " << questList[i].getQuestName();
            questList[i].registerKill(target);
        }
    }

}

void questsPlusController::registerChatLog(QStringList chatStrings, ChatType::ChatType type)
{

    for ( int i = 0; i < chatStrings.count( ); i++)
    {
        if ( ( i == 1 ) && ( chatStrings[i] == appConfig->CharacterName ))
        {
            chatStrings[i] = "SELF";
        }
        if ( ( i == 2 ) && ( chatStrings[i] == appConfig->CharacterName ) )
        {
            chatStrings[i] = "WHISPER";
        }
    }
    //Incoming data is a QStringList put out by QRegExp, specific to the chattype
    //That way our main window handles as much parsing as possible (for future language conversions)
    //(0) should be the entire matched regexp, with 1-x being the count
    for ( int i = 0; i < questList.count(); i++ )
    {
        switch (type)
        {
            case ChatType::Chat:
            case ChatType::Whisper:
            {
                if ( questList[i].hasChatTRIGGER() )
                {
                    questList[i].registerChat(chatStrings, type);
                }
                break;
            }
            case ChatType::Spell_Cast:
            {
                if ( questList[i].hasSpellCastTRIGGER() )
                {
                    questList[i].registerSpellCast(chatStrings);
                }
                break;
            }
            default:
            {
                break;
            }
        }
    }

}

void questsPlusController::registerLoot(QString item, int amount)
{
    for ( int i = 0; i < questList.count(); i++ )
    {
        if ( questList[i].hasLootTRIGGER() )
        {
            questList[i].registerLoot(item,amount);
        }
    }
};
