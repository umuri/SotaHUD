#ifndef QUESTSPLUSQUEST_H
#define QUESTSPLUSQUEST_H

#include <QObject>
#include <QDateTime>
#include <QDebug>
#include <QMediaPlayer>
#include <QMediaPlaylist>

//This is stupid but needed for a forward reference
class questsPlusQuest;

namespace ChatType
{
    enum ChatType
    {
        //FINDME: TRIGGERADD
        Whisper,
        Emote,
        Chat,
        Spell_Cast,
        Loot
    };
}

enum TRIGGERType
{
    //FINDME: TRIGGERADD
    Required_Quest_Stage,
    Player_At_Location,
    Kill,
    Player_Talks_To_NPC,
    Repeatable,
    Goals_Complete,
    Goal_Complete,
    Goal_Not_Complete,
    Quest_Load,
    Event_Timed,
    Chat,
    Chat_Channel,
    Spell_Cast,
    Emote,
    Loot,
    UNKNOWN_TRIGGER
};






enum EVENTType
{
    //FINDME: EVENTADD
    Advance_To_Next_Stage,
    Add_Goal,
    Advance_Goal,
    Remove_Goal,
    Clear_Goals,
    UpdateQuestLog,
    CompleteQuest,
    Popup_Message,
    Popup_Image,
    Play_Sound,
    Debug,
    UNKNOWN_EVENT
};



struct questTRIGGER
{
    TRIGGERType type;
    QList<QString> args;
    int progress = 0;
    bool complete = false;
    bool blocking = false;
};
struct questEVENT
{
    EVENTType type;
    QList<QString> args;
};
struct questBLOCK
{

    QList<questEVENT> EVENTs;
    QList<questTRIGGER> TRIGGERs;
    bool fired = false;
    bool repeatable = false;

    int repeatTime = 0;
    int BLOCKNumber = -1;
    QDateTime lastFired;

    bool isEventuallyFirable() {
        //This one is just for the advance Quest check, just in case it's repeatable but currently inactive
        //return true;
        bool currentFirable = true;
        if ( fired ) //We've already fired this
        {
            if ( !repeatable ) //And its not repeatable
            {
                return false;
            }
        }
        if ( TRIGGERs.count() == 0 ) { return false; }

        for ( int i = 0; i < TRIGGERs.count(); i++ )
        {
            if (( TRIGGERs[i].type == TRIGGERType::Required_Quest_Stage ) && ( TRIGGERs[i].complete == false ) ) //Not on the right stage
            {
                currentFirable = false;
            }
        }
        return currentFirable;
    }

    bool isFirable() {
        //return true;
        bool currentFirable = true;
        if ( fired ) //We've already fired this
        {
            if ( !repeatable )
            {
                return false;
            }
            else if ( lastFired.secsTo(QDateTime::currentDateTime()) < repeatTime ) //Repeatable, has enough time passed?
            {
                return false;
            }
        }
        if ( TRIGGERs.count() == 0 ) { return false; }

        for ( int i = 0; i < TRIGGERs.count(); i++ )
        {
            if (( TRIGGERs[i].blocking == true) && ( TRIGGERs[i].complete == false ) )
            {
                currentFirable = false;
            }
        }
        return currentFirable;
    }

    bool isComplete() {
        bool currentComplete = true;
        if ( TRIGGERs.count() == 0 ) { return false; }
        if ( !isFirable() ) { return false; }
        for ( int i = 0; i < TRIGGERs.count(); i++ )
        {
            if ( ( TRIGGERs[i].complete == false ) )
            {
                currentComplete = false;
            }
        }
        return currentComplete;
    }
    int indexFirableTrigger(TRIGGERType targetTRIGGER, int start = 0)
    {
        if ( !isFirable() )
            { return -1; }
        for ( int i = start; i < TRIGGERs.count();i++ )
        {
            if ( TRIGGERs[i].type == targetTRIGGER )
            {
                return i;
            }
        }
        return -1;
    }

    int indexTrigger(TRIGGERType targetTRIGGER, int start = 0)
    {
        for ( int i = start; i < TRIGGERs.count();i++ )
        {
            if ( TRIGGERs[i].type == targetTRIGGER )
            {
                return i;
            }
        }
        return -1;
    }

    static QString getTRIGGERType(TRIGGERType TRIGGER)
    {
          //FINDME: TRIGGERADD
          switch (TRIGGER)
          {
              case Required_Quest_Stage: { return "Required_Quest_Stage";   break;}
              case Player_At_Location: { return "Player_At_Location";   break;}
              case Kill: { return "Kill"; break; }
              case Player_Talks_To_NPC: { return "Player_Talks_To_NPC"; break; }
              case Repeatable: { return "Repeatable"; break; }
              case Goals_Complete: { return "Goals_Complete"; break; }
              case Goal_Complete: { return "Goal_Complete"; break; }
              case Goal_Not_Complete: { return "Goal_Not_Complete"; break; }
              case Quest_Load: { return "Quest_Load"; break; }
              case Event_Timed: { return "Event_Timed"; break; }
              case Chat: { return "Chat"; break; }
              case Chat_Channel: { return "Chat_Channel"; break; }
              case Spell_Cast: { return "Spell_Cast"; break; }
              case Emote: { return "Emote"; break; }
              case Loot: { return "Loot"; break; }
              case UNKNOWN_TRIGGER:
              default:
            { return "UNKNOWN_TRIGGER"; break; }

      }
    }
    static QString getEVENTType(EVENTType EVENT)
    {
         //FINDME: EVENTADD
          switch (EVENT)
          {
              case Advance_To_Next_Stage: { return "Advance_To_Next_Stage";   break;}
              case Add_Goal: { return "Add_Goal";   break;}
              case Advance_Goal: { return "Advance_Goal"; break; }
              case Remove_Goal: { return "Remove_Goal"; break; }
              case Clear_Goals: { return "Clear_Goals"; break; }
              case UpdateQuestLog: { return "UpdateQuestLog"; break; }
              case CompleteQuest: { return "CompleteQuest"; break; }
              case Popup_Message: { return "Popup_Message"; break; }
              case Popup_Image: { return "Popup_Image"; break; }
              case Play_Sound: { return "Play_Sound"; break; }
              case Debug: { return "Debug"; break; }
              case UNKNOWN_EVENT:
              default:
            { return "UNKNOWN_EVENT"; break; }

      }
    }


    QString toString()
    {
        QString temp;
        temp = QString::number(BLOCKNumber);
        temp.append("|?|");
        temp.append((fired ? "1" : "0"));
        temp.append("|?|");
        temp.append(lastFired.toString());
        temp.append("|?|");
        return temp;
    }
    //blockID << "|?|" << fired << "|?|" << firedDate << "|?|"

};
struct questLogEntry
{
    QString entry;
    QDateTime timestamp;
    QString toString()
    {
        QString temp;
        temp = entry;
        temp.append("|?|");
        temp.append(timestamp.toString());
        temp.append("|?|");
        return temp;
    }
};
struct questGoal
{
    QString goalName;
    QString goalID;
    int goalTarget = -1;
    int goalStatus = -1;
    int goalNumber = -1;
    questsPlusQuest * parent = NULL;
    bool isComplete()
    {
        if ( goalStatus >= goalTarget )
        {return true;}
        else
        {return false;}
    }
    QString getGoalStatus()
    {
        if ( isComplete() )
        {
            return "[Done ]";
        }

        float progress = (float)goalStatus / (float)goalTarget;
        QString output;
        int pcount = progress * 5;
        output.fill('=',pcount);
        output = output.leftJustified(5,' ');
        output = "[" + output + "]";
        return output;
    }

    QString toString()
    {
        QString temp;
        temp = goalID;
        temp.append("|?|");
        temp.append(goalName);
        temp.append("|?|");
        temp.append(QString::number(goalStatus));
        temp.append("|?|");
        temp.append(QString::number(goalTarget));
        temp.append("|?|");
        return temp;
    }
};



class questsPlusQuest
{

private:

    bool StillValid = true;
    QString ErrorString;
    int QuestStage = 0; //Quest always starts at stage 1
    QList<questBLOCK> BLOCKList;
    bool newQuest = true;
    bool completed = false;
    bool killTRIGGER = false;
    bool posTRIGGER = false;
    bool timedTRIGGER = false;
    bool chatTRIGGER = false;
    bool spellCastTRIGGER = false;
    bool lootTRIGGER = false;


    bool expanded = false;
    QList<questLogEntry> questLog;
    QString questName;
    QList<questGoal> goalList;
    QMediaPlayer * soundChannels[6];
    QString soundChannelSources[6];
    QMediaPlaylist * soundPlaylistUninterrupt;
    QMediaPlaylist * soundPlaylistLoop;

    void playQuestSound(QString file, int channel);
    bool parseBlock(QString);
    QList<QString> parseARGS(QString input);
    void AdvanceQuestStage(int);
    void fireBlock(int BlockNumber);
    void completeQuest();

    void updateQuestLog(QString newEntry);
    bool loadQuest();
    bool createSoundChannel(int channel);
    void recalculateGoalsTriggers();
public:
    questsPlusQuest();
    questsPlusQuest(QString questFolderName);
    bool isValid();
    bool isCompleted();
    bool hasPosTRIGGER();
    bool hasKillTRIGGER();
    QString getQuestName();
    QString getQuestStatus();
    int getGoalsCount();
    QString getGoalName(int goalNumber);
    QString getGoalStatus(int goalNumber);
    questGoal *getGoalPtr(int goalNumber);



    bool DEBUG = false;

    QString getError();
    TRIGGERType getTRIGGERType(QString TRIGGERName);
    EVENTType getEVENTType(QString EVENTName);
    void registerKill(QString KillName);
    questsPlusQuest(const questsPlusQuest &old);
    void registerCurrentLocation(QString SceneName, double x, double y, double z);
    QString toString();
    void saveQuest();
    int getQuestStage();
    bool isExpanded();
    void setExpanded(bool newStatus);
    QString getBlockDebug();
    void tickTimers();
    bool hasTimedTRIGGER();
    ~questsPlusQuest();
    bool hasChatTRIGGER();
    bool hasSpellCastTRIGGER();
    void registerChat(QStringList chatStrings, ChatType::ChatType);
    void registerSpellCast(QStringList chatStrings);
    bool hasLootTRIGGER();
    void registerLoot(QString item, int amount);
    QList<questLogEntry> getQuestLog();
};

#endif // QUESTSPLUSQUEST_H
