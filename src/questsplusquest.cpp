#include "questsplusquest.h"
#include <QDir>
#include <QFile>
#include <QDebug>
#include <QTextStream>
#include <QMessageBox>
#include <QDialog>
#include <QPixMap>
#include <QLabel>




questsPlusQuest::questsPlusQuest()
{
    //This should never fire!
    BLOCKList.clear();
}

questsPlusQuest::questsPlusQuest(QString questFolderName)
{
    BLOCKList.clear();
    questName = questFolderName;

/*  Random goal testing code
    for ( int i = 0; i < 30; i++)
    {
        if ( rand()%10 > 8 )
        {
          questGoal newGoal;
          newGoal.goalName = "Bacon" + QString::number(rand()%40);
          newGoal.goalNumber=goalList.count();
          newGoal.parent = this;
          //qDebug() << "Goal added: " << questName << " :    " << newGoal.goalName << "  " << newGoal.goalNumber << " " << (void *)this << " " << (void *)newGoal.parent;
          goalList.append(newGoal);
        }
    }
*/

    //Initialize Sound Channels
    //Null them out until they are used;
    for ( int i = 0; i < 6; i++)
    {
        QMediaPlayer * temp = NULL;
        soundChannels[i] = temp;
        soundChannelSources[i] = "";
    }

    QString questPath = QDir::currentPath() + "/quests/" + questFolderName + "/Quest.udf";
    QFile questFile(questPath);

    if ( !questFile.exists() )
    {
        StillValid = false;
        ErrorString = "No file Quest.udf found in quest folder [" + questFolderName + "]";
        return;
    }

    questFile.open(QIODevice::ReadOnly);

    QTextStream questIn(&questFile);

    bool found = false;
    while(!questIn.atEnd())
    {


       QString line = questIn.readLine();
        // Pipe is escaped by two \\, so this looks ugly. Sorry.
       QRegExp rx("\\|\\|\\|\\|(.*)\\|\\|\\|\\|", Qt::CaseSensitive);

       if ( rx.indexIn(line,0) > -1 )
       {
         //qDebug() << rx.cap(1) << endl;

         if ( parseBlock(rx.cap(1)) )
         {
             found = true;
         }

       }
    }

    if ( found == false )
    {
        StillValid = false;
        ErrorString = "No valid BLOCKs found in quest";
    }





    if ( loadQuest() ) //Restore the old state if we have one
    {
        newQuest = false;
    }

    //Process our initializers for new quests
    if ( newQuest )
    {


        for ( int i = 0; i < BLOCKList.count(); ++i)
        {
            if ( BLOCKList[i].indexTrigger(TRIGGERType::Quest_Load) >= 0 )
            {
                //qDebug() << "Firing Quest Load Block: " << i;
                fireBlock(i);
                break;
            }
        }
        AdvanceQuestStage(1);
        newQuest = false;
    }


    /*
    // DEBUG PROCESSING
    for ( int i = 0; i < BLOCKList.length(); i++ )
    {
        qDebug() << endl << "[Block " << i << "] Fired["<< BLOCKList[i].fired << "] Firable["<< BLOCKList[i].isisFirable() << "]";
        qDebug() << "TRIGGERs:";
        for ( int j = 0; j < BLOCKList[i].TRIGGERs.length(); j++)
        {
            QString argsTemp = "";
            for ( int k = 0; k < BLOCKList[i].TRIGGERs[j].args.count(); k++)
            {
                argsTemp += " | ";
                argsTemp += BLOCKList[i].TRIGGERs[j].args[k];
                argsTemp += " | ";
            }
            qDebug() << BLOCKList[i].TRIGGERs[j].type << " " << argsTemp;
        }
        qDebug() << "EVENTs:";
        for ( int j = 0; j < BLOCKList[i].EVENTs.length(); j++)
        {

            QString argsTemp = "";
            for ( int k = 0; k < BLOCKList[i].EVENTs[j].args.count(); k++)
            {
                argsTemp += " | ";
                argsTemp += BLOCKList[i].EVENTs[j].args[k];
                argsTemp += " | ";
            }
            qDebug() << BLOCKList[i].EVENTs[j].type << " " << argsTemp;
        }
    }
    */

    //qDebug() << this;

}

questsPlusQuest::~questsPlusQuest()
{
    //Clean up any sound channels when we destruct this one.
    for ( int i = 0; i < 6; i++ )
    {
        if ( soundChannels[i] )
        {
            delete soundChannels[i];
        }
    }
}


bool questsPlusQuest::parseBlock(QString currentBLOCK)
{

    // BLOCKs are defined by four pipes ( | )  characters on either end of the line
    // Within the block TRIGGERs and EVENTs are seperated by ||
    // Arguments within TRIGGERs and EVENTs are seperated by |

    QString currentSegment;
    questBLOCK newBLOCK;


    int pos = 0;
    int endPos = 0;
    //Find each instance of ||
    endPos = currentBLOCK.indexOf("||",pos);

    int count = 1;
    while ( pos > -1 )
    {
       currentSegment = currentBLOCK.mid(pos,endPos-pos);
       //We have now found our segment, lets process it.
       //qDebug() << "Segment " << count << ": " << currentSegment << endl;

       //PSUEDO:  Seperate out TRIGGERs and BLOCKs, store them into their appropriate struct

       if ( currentSegment.indexOf("TRIGGER:") == 0 ) //If starts with TRIGGER:, it's a TRIGGER
       {
           //qDebug() << "TRIGGER FOUND: " << currentSegment << endl;

           currentSegment = currentSegment.mid(8,-1);

           int pos = currentSegment.indexOf('(');
           QString triggerName = currentSegment.mid(0,pos);
           QString ARGS = currentSegment.mid(pos+1,currentSegment.length() - pos -1 );
           ARGS = ARGS.trimmed();
           if ( ARGS[ARGS.length()-1] == ')' ) //Trim away trailing ) and whitespaces.
           {
               ARGS = ARGS.mid(0,ARGS.length()-1);
           }
            //qDebug() << "[QPQ][PB]" << ARGS;
           //qDebug() << triggerName << ":" << ARGS << endl;
           questTRIGGER newTRIGGER;
           newTRIGGER.type = getTRIGGERType(triggerName);
           //qDebug() << "[QPQ][parseBlock]" << triggerName << ":" << newTRIGGER.type;
           newTRIGGER.args = parseARGS(ARGS);


           //INITIALIZE TRIGGERS
           //FINDME: TRIGGERADD
           switch(newTRIGGER.type)
           {
                case Required_Quest_Stage:
                    {
                        newTRIGGER.blocking = true;
                       break;
                    }
                case Player_At_Location:
                    {
                        newTRIGGER.blocking = true;
                        break;
                    }
                case Goal_Not_Complete:
                    {
                        newTRIGGER.blocking = true;
                        newTRIGGER.complete = true;
                        break;
                    }
                case Repeatable:
                    {
                        newBLOCK.repeatable = true;
                        newBLOCK.repeatTime = newTRIGGER.args[0].toInt();
                        newTRIGGER.complete = true;
                    }
                default:
                    break;
            }
           newBLOCK.TRIGGERs.append(newTRIGGER);
       }
       else if ( currentSegment.indexOf("EVENT:") == 0 ) //If starts with EVENT:, it's an EVENT
       {
           currentSegment = currentSegment.mid(6,-1);

           int pos = currentSegment.indexOf('(');
           QString eventName = currentSegment.mid(0,pos);
           QString ARGS = currentSegment.mid(pos+1,currentSegment.length() - pos - 1 );
           ARGS = ARGS.trimmed();
           if ( ARGS[ARGS.length()-1] == ')' ) //Trim away trailing ) and whitespaces.
           {
               ARGS = ARGS.mid(0,ARGS.length()-1);
           }
           //qDebug() << "EVENT FOUND: " << currentSegment << endl;
           questEVENT newEVENT;
           newEVENT.type = getEVENTType(eventName);
           newEVENT.args = parseARGS(ARGS);

           //FINDME: EVENTADD
            switch(newEVENT.type) //Initialization, if we use this event and it needs preloading this is where it goes
            {
                case EVENTType::Play_Sound:
                    {
                        //Intialize all usable sound channels here so they don't break when initializing while others are playing
                        //What i mean by this is, If you instantiate a new QMediaPlayer while another one is loading a file or playing one, it crashes.
                        createSoundChannel(newEVENT.args[1].toInt());
                        break;
                    }
                default:
                    break;
            }

           newBLOCK.EVENTs.append(newEVENT);


       }
       else // THIS SHOULD NOT BE HIT, nothing in blocks besides TRIGGER and EVENT. MALFORMED
       {
           //qDebug() << "INVALID: " << currentSegment << endl;
           StillValid = false;
           ErrorString = "INVALID BLOCK SEGMENT: [" + currentSegment + "] in BLOCK [" + currentBLOCK + "]";
       }


       count++;

       pos = endPos+2;

       endPos = currentBLOCK.indexOf("||",pos);
       if ( endPos == -1 ) //We are at the end so the last segment is everything that's left
       {
           endPos = currentBLOCK.length();
       }

       if ( endPos < pos ) //We have gone past the end of line now, we're done
       {
           pos = -1;
       }

    }

    if ( ( newBLOCK.EVENTs.count() > 0 ) && ( newBLOCK.TRIGGERs.count() > 0 ) )
    {
        newBLOCK.BLOCKNumber = BLOCKList.count();
        BLOCKList.append(newBLOCK);
        return true;
    }

    return false;
}

void questsPlusQuest::AdvanceQuestStage(int newStage = -1)
{
    qDebug() << "[QPQ][AQS] " << questName;
    if ( newStage == -1 )
    {
        QuestStage++;
    }
    else
    {
        QuestStage = newStage;
    }

    //FINDME: Triggeradd  - Just in case it needs to have a new tracker boolean put in.
    //If you add one here don't forget to update the copy constructor, save, and load code
    //qDebug() << "[QPQ]AdvanceQuestStage: Cleaning Triggers on Quest " << questName;
    timedTRIGGER = false;
    killTRIGGER = false;
    posTRIGGER = false;
    chatTRIGGER = false;
    spellCastTRIGGER = false;
    lootTRIGGER = false;

    for ( int i = 0; i < BLOCKList.count();i++)
    {
        for ( int j = 0; j < BLOCKList[i].TRIGGERs.count(); j++)
        {
            if ( BLOCKList[i].TRIGGERs[j].type == Required_Quest_Stage )
            {
                if (  BLOCKList[i].TRIGGERs[j].args[0].toInt() == QuestStage )
                {
                    BLOCKList[i].TRIGGERs[j].complete=true;
                }
                else //Not our current stage, disable all firability checks
                {
                    BLOCKList[i].TRIGGERs[j].complete=false;
                }
            }
        }
    }


    for ( int i = 0; i < BLOCKList.count();i++)
    {
        //qDebug () << "[QPQ][AQS] BLOCK:" << i << "   isEventuallyFirable:" << BLOCKList[i].isEventuallyFirable();
        if ( BLOCKList[i].isEventuallyFirable() )
        {
            questBLOCK * currentBLOCK = &BLOCKList[i];
            for ( int j = 0; j < currentBLOCK->TRIGGERs.count(); j++)
            {
                questTRIGGER * currentTRIGGER = &currentBLOCK->TRIGGERs[j];
                //qDebug() << "[QPQ]" << questName << ":[" << i << ":" << j << "]" << currentTRIGGER->type << " with args " << currentTRIGGER->args;

                switch ( currentTRIGGER->type )
                {
                    case TRIGGERType::Player_At_Location:
                    {
                        posTRIGGER = true;
                        break;
                    }
                    case TRIGGERType::Event_Timed:
                    {
                        timedTRIGGER = true;
                        break;
                    }
                    case TRIGGERType::Spell_Cast:
                    {
                        spellCastTRIGGER = true;
                        break;
                    }
                    case TRIGGERType::Chat:
                    case TRIGGERType::Chat_Channel:
                    {
                        chatTRIGGER = true;
                        break;
                    }
                    case TRIGGERType::Kill:
                    {
                        killTRIGGER = true;
                        break;
                    }
                    case TRIGGERType::Loot:
                    {
                        lootTRIGGER = true;
                        break;
                    }
                    default:
                    {
                            break;
                    }
                }
            }
        }
    }

    //qDebug() << questName << ":" << posTRIGGER << ":" << killTRIGGER;

}

bool questsPlusQuest::isValid()
{
    return StillValid;
}

bool questsPlusQuest::isCompleted()
{
    return completed;
}

bool questsPlusQuest::hasPosTRIGGER()
{
    return posTRIGGER;
}

bool questsPlusQuest::hasKillTRIGGER()
{
    return killTRIGGER;
}
bool questsPlusQuest::hasTimedTRIGGER()
{
    return timedTRIGGER;
}

bool questsPlusQuest::hasChatTRIGGER()
{
    return chatTRIGGER;
}

bool questsPlusQuest::hasSpellCastTRIGGER()
{
    return spellCastTRIGGER;
}

bool questsPlusQuest::hasLootTRIGGER()
{
    return lootTRIGGER;
}


QString questsPlusQuest::getQuestName()
{
    return questName;
}

QString questsPlusQuest::getQuestStatus()
{
    if ( questLog.count() < 1 )
    {
        return "Quest Not Started";
    }
    return questLog[questLog.count()-1].entry;
}

int questsPlusQuest::getGoalsCount()
{
    return goalList.count();
}

QString questsPlusQuest::getGoalName(int goalNumber)
{
    if ( goalNumber < 0 )
    {
        return "ERROR negative index";
    }
    if ( goalNumber > goalList.count() )
    {
        return "ERROR out of bounds";
    }
    else
    {
        return goalList[goalNumber].goalName;
    }
}

QString questsPlusQuest::getGoalStatus(int goalNumber)
{
    if ( goalNumber < 0 )
    {
        return "ERROR negative index";
    }
    if ( goalNumber > goalList.count() )
    {
        return "ERROR out of bounds";
    }
    return goalList[goalNumber].getGoalStatus();
}

questGoal *questsPlusQuest::getGoalPtr(int goalNumber)
{
    if ( goalNumber < 0 )
    {
        return NULL;
    }
    if ( goalNumber > goalList.count() )
    {
        return NULL;
    }
    else
    {
        return &goalList[goalNumber];
    }
}
QString questsPlusQuest::getError()
{
    return ErrorString;
}

QList<QString> questsPlusQuest::parseARGS(QString input)
{
    //qDebug() << "[QPQ][PA]" << input << endl;
    bool end = false;
    QList<QString> tempARGS;
    while ( ( input.length() > 0 ) && ( !end ) )
    {
        //ARGS seperated by Pipe | character
        int endpos = input.indexOf('|');
        //Do we have only one argument? if so call this good.
        if ( endpos < 0 )
        {
            end = true;            
            tempARGS.append(input);
        }
        else if ( endpos > 0 ) //We have an argument plus some ending weight
        {
            while ( input[endpos-1] == '\\' ) //This pipe is escaped, bypass it
            {
                endpos = input.indexOf('|',endpos+1);
            }
            //Add arg to string
            tempARGS.append(input.left(endpos));
            //Advance to next arg
            input = input.mid(endpos+1,-1);
        }
        else //Next character is pipe, invalid. Try to be nice and advance to next one
        {
            if ( input.length() > 1 )
            {
                input.mid(1,-1);
            }
            else //We assume they made a horrible mistake, and only put a single | as the argument, without an escape. So we fix it for them
            {
                input = "";
                tempARGS.append("|");
            }
        }
    }

    /*qDebug() << "ARGUMENTS PARSED: ";
    for ( int i = 0; i < tempARGS.count(); i++ )
    {
        qDebug() << " ||  " << tempARGS[i] << " || ";
    }
    qDebug()  << endl;*/
    //qDebug() << "[QPQ][QA] ARGUMENTS PARSED: ";
    //qDebug() << tempARGS;

    return tempARGS;
}


TRIGGERType questsPlusQuest::getTRIGGERType(QString TRIGGERName)
{
    //FINDME: TRIGGERADD
    if ( TRIGGERName == "Required_Quest_Stage" )
    {  return TRIGGERType::Required_Quest_Stage; }
    else if (  TRIGGERName == "Player_At_Location" )
    { return TRIGGERType::Player_At_Location; }
    else if (  TRIGGERName == "Kill" )
    { return TRIGGERType::Kill; }
    else if (  TRIGGERName == "Player_Talks_To_NPC" )
    { return TRIGGERType::Player_Talks_To_NPC; }
    else if (  TRIGGERName == "Repeatable" )
    { return TRIGGERType::Repeatable; }
    else if ( TRIGGERName == "Quest_Load" )
    { return TRIGGERType::Quest_Load; }
    else if ( TRIGGERName == "Goals_Complete" )
    { return TRIGGERType::Goals_Complete; }
    else if ( TRIGGERName == "Goal_Complete" )
    { return TRIGGERType::Goal_Complete; }
    else if ( TRIGGERName == "Goal_Not_Complete" )
    { return TRIGGERType::Goal_Not_Complete; }
    else if ( TRIGGERName == "Event_Timed" )
    { return TRIGGERType::Event_Timed; }
    else if ( TRIGGERName == "Chat" )
    { return TRIGGERType::Chat; }
    else if ( TRIGGERName == "Chat_Channel" )
    { return TRIGGERType::Chat_Channel; }
    else if ( TRIGGERName == "Spell_Cast" )
    { return TRIGGERType::Spell_Cast; }
    else if ( TRIGGERName == "Emote" )
    { return TRIGGERType::Emote; }
    else if ( TRIGGERName == "Loot" )
    { return TRIGGERType::Loot; }
    else
    {
        StillValid = false;
        ErrorString = "Uknown TRIGGER: " + TRIGGERName;
        return TRIGGERType::UNKNOWN_TRIGGER;
    }
}

EVENTType questsPlusQuest::getEVENTType(QString EVENTName)
{
    //FINDME: EVENTADD
    if ( EVENTName == "Advance_To_Next_Stage" )
    { return EVENTType::Advance_To_Next_Stage; }
    else if ( EVENTName == "Add_Goal" )
    { return EVENTType::Add_Goal; }
    else if ( EVENTName == "Advance_Goal" )
    { return EVENTType::Advance_Goal; }
    else if ( EVENTName == "UpdateQuestLog" )
    { return EVENTType::UpdateQuestLog; }
    else if ( EVENTName == "CompleteQuest" )
    { return EVENTType::CompleteQuest; }
    else if ( EVENTName == "Remove_Goal" )
    { return EVENTType::Remove_Goal; }
    else if ( EVENTName == "Clear_Goals" )
    { return EVENTType::Clear_Goals; }
    else if ( EVENTName == "Play_Sound" )
    { return EVENTType::Play_Sound;  }
    else if ( EVENTName == "Popup_Message" )
    { return EVENTType::Popup_Message; }
    else if ( EVENTName == "Popup_Image" )
    { return EVENTType::Popup_Image; }
    else if ( EVENTName == "Debug" )
    { return EVENTType::Debug; }

    else
    {
        StillValid = false;
        ErrorString = "Uknown EVENT: " + EVENTName;
        return EVENTType::UNKNOWN_EVENT;
    }
}

void questsPlusQuest::tickTimers()
{
    for ( int i = 0; i < BLOCKList.count(); ++i)
    {
        questBLOCK * currentBLOCK = &BLOCKList[i];
        int targetTRIGGER = currentBLOCK->indexFirableTrigger(TRIGGERType::Event_Timed);
        while ( targetTRIGGER >= 0 )
        {
              questTRIGGER * currentTRIGGER = &(currentBLOCK->TRIGGERs[targetTRIGGER]);

              currentTRIGGER->progress++;
              if ( currentTRIGGER->progress >= currentTRIGGER->args[0].toInt() )
              {
                  currentTRIGGER->complete = true;
                  if ( currentBLOCK->isComplete() )
                  {
                      fireBlock(i);
                  }

              }
              targetTRIGGER = currentBLOCK->indexFirableTrigger(TRIGGERType::Event_Timed,targetTRIGGER+1);
        }

    }
}


void questsPlusQuest::fireBlock(int BlockNumber)
{
    questBLOCK * currentBLOCK = &BLOCKList[BlockNumber];
    //We trigger this to fire all EVENTs within the block whenever the block trigger conditions are met
    currentBLOCK->fired = true;
    currentBLOCK->lastFired = QDateTime::currentDateTime();

    //Reset any trigger progress just in case
    for ( int i = 0; i < currentBLOCK->TRIGGERs.count(); ++i)
    {
        currentBLOCK->TRIGGERs[i].progress = 0;
    }

    //Loop through each event
    for ( int i = 0; i < currentBLOCK->EVENTs.count(); i++)
    {
        questEVENT * currentEVENT = &currentBLOCK->EVENTs[i];


        //Fire away!
        switch(currentEVENT->type)
        {
            //FINDME: EVENTADD
            case Advance_To_Next_Stage:
                {
                    int newStage = -1;
                    if ( currentEVENT->args.count() > 0 ) //If they don't include any args
                    {
                       newStage = currentEVENT->args[0].toInt();
                    }

                    if ( ( newStage < 1 ) || ( newStage > 9999 ))
                    {  newStage = -1; }
                    AdvanceQuestStage(newStage);
                    //qDebug() << "Stage advanced, currently: " << QuestStage;
                    break;
                }
            case Add_Goal:
                {
                    expanded = true;
                    questGoal newGoal;
                    newGoal.goalID = currentEVENT->args[0];
                    newGoal.goalName = currentEVENT->args[1];
                    newGoal.goalTarget = currentEVENT->args[2].toInt();
                    newGoal.goalStatus = 0;
                    newGoal.goalNumber=goalList.count();
                    newGoal.parent = this;
                    goalList.append(newGoal);
                    break;
                }
            case Advance_Goal:
                {
                    QString GoalIDToComplete = currentEVENT->args[0]; //Stored to make code more readable.
                    bool completed = false;
                    int totalCompleted = 0;
                    for ( int i = 0; i < goalList.count(); ++i)
                    {
                        if ( goalList[i].goalID == GoalIDToComplete)
                        {
                            goalList[i].goalStatus++;

                            if ( goalList[i].isComplete() ) //We've completed a new goal, re-process goal TRIGGERs
                            {
                                completed = true;
                            }
                        }
                        if ( goalList[i].isComplete() )
                        {
                            ++totalCompleted;
                        }
                    }

                    //qDebug() << "Goal advanced, completed? " << completed << " and total completes " << totalCompleted;
                    if ( completed ) //The advanced goal is considered complete, check for fires.
                    {
                        //First do the Goals_Complete TRIGGER, which checks for a sum.
                        for ( int i = 0; i < BLOCKList.count(); ++i)
                        {
                            //First do the Goals_Complete trigger, then do the Goal_Complete trigger
                            questBLOCK * currentBLOCK = &BLOCKList[i];
                            int targetTRIGGER = currentBLOCK->indexFirableTrigger(TRIGGERType::Goals_Complete);
                            while ( targetTRIGGER >= 0 )
                            {
                                  questTRIGGER * currentTRIGGER = &(currentBLOCK->TRIGGERs[targetTRIGGER]);

                                 if ( totalCompleted >= currentTRIGGER->args[0].toInt() )
                                 {
                                     currentTRIGGER->complete = true;
                                     if ( currentBLOCK->isComplete() )
                                     {
                                         fireBlock(currentBLOCK->BLOCKNumber);
                                     }
                                 }
                                 targetTRIGGER = currentBLOCK->indexFirableTrigger(TRIGGERType::Goals_Complete,targetTRIGGER+1);
                            }

                            //Check for completion of specific goal
                            targetTRIGGER = currentBLOCK->indexFirableTrigger(TRIGGERType::Goal_Complete);
                            while ( targetTRIGGER >= 0 )
                            {
                                  questTRIGGER * currentTRIGGER = &(currentBLOCK->TRIGGERs[targetTRIGGER]);
                                 if ( currentTRIGGER->args[0] == GoalIDToComplete) //Compare the GoalID we just advanced with the Arg0 of the Goal_Complete TRIGGER
                                 {
                                     currentTRIGGER->complete = true;
                                     if ( currentBLOCK->isComplete() )
                                     {
                                         fireBlock(currentBLOCK->BLOCKNumber);
                                     }
                                 }
                                 targetTRIGGER = currentBLOCK->indexFirableTrigger(TRIGGERType::Goal_Complete,targetTRIGGER+1);
                            }

                            //Check for marking any Goal_Not_Complete triggers as failed
                            targetTRIGGER = currentBLOCK->indexFirableTrigger(TRIGGERType::Goal_Not_Complete);
                            while ( targetTRIGGER >= 0 )
                            {
                                  questTRIGGER * currentTRIGGER = &(currentBLOCK->TRIGGERs[targetTRIGGER]);
                                 if ( currentTRIGGER->args[0] == GoalIDToComplete) //Compare the GoalID we just advanced with the Arg0 of the Goal_Complete TRIGGER
                                 {
                                     currentTRIGGER->complete = false; //Goal is complete, so now the not complete trigger is false
                                 }
                                 targetTRIGGER = currentBLOCK->indexFirableTrigger(TRIGGERType::Goal_Not_Complete,targetTRIGGER+1);
                            }

                        }
                    }
                }
                break;
            case Remove_Goal:
                {
                    bool removed = false;
                    for ( int i = 0; i < goalList.count(); ++i)
                    {
                        if ( removed )
                        {
                            goalList[i].goalNumber = i;
                        }
                        else if ( goalList[i].goalID == currentEVENT->args[0])
                        {
                            goalList.removeAt(i);
                            removed = true;
                            --i; //Backup one spot and start re-numbering goals
                        }
                    }
                    recalculateGoalsTriggers();
                    break;
                }

            case Clear_Goals:
                {
                    goalList.clear();
                    recalculateGoalsTriggers();
                    break;
                }

            case UpdateQuestLog:
                {
                    updateQuestLog(currentEVENT->args[0]);
                    //qDebug() << "Updated Quest Log. Current Entry count: " << questLog.count();
                    break;
                }
            case CompleteQuest:
                {
                    //qDebug() << "YAY YOU COMPLETED THE QUEST!";
                    AdvanceQuestStage(9999);
                    completeQuest();

                    break;
                }
            case Popup_Message:
                {
                    QString message;
                    QString title;
                    title = questName;
                    title += " - ";
                    title += currentEVENT->args[0];
                    message = currentEVENT->args[1];



                    QMessageBox questPopup(QMessageBox::NoIcon, title, message,
                                                    QMessageBox::Ok);
                    questPopup.setTextInteractionFlags(Qt::TextSelectableByMouse);

                    questPopup.setWindowFlags(questPopup.windowFlags()|Qt::WindowStaysOnTopHint);

                    questPopup.exec();
                    break;
                }
            case Popup_Image:
                {

                    //The below is ugly, because i didn't feel like making a whole subclass to do it.
                    //Suffice to say adjustSize() didn't work, so we had to manually specify sizings.
                    QDialog popup;
                    QString title = questName + "-" + currentEVENT->args[0];
                    popup.setWindowTitle(title);
                    popup.setMaximumHeight(10000);
                    popup.setMaximumWidth(10000);
                    QLabel* lblImage = new QLabel(&popup);
                    QPixmap image;
                    popup.setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
                    QString imagePath = QDir::currentPath() + "/quests/" + questName + "/" + currentEVENT->args[1];
                    image.load(imagePath);
                    lblImage->resize(image.width(),image.height());
                    lblImage->setPixmap(image);
                    popup.setWindowFlags(popup.windowFlags()|Qt::WindowStaysOnTopHint);
                    popup.resize(image.width(),image.height());
                    popup.exec();
                    popup.adjustSize();
                    //qDebug() << "[QPQ]Loading Image: " << imagePath;


                    break;
                }
            case Play_Sound:
                {
                    //Arg1 - Sound file name
                    //Arg2 - Channel
                    //Channel 5 loops automatically
                    //Channel 6 uses a queue and doesn't interrupt.
                    playQuestSound(currentEVENT->args[0],currentEVENT->args[1].toInt());
                    break;
                }
            case Debug:
                {
                    DEBUG = currentEVENT->args[0].toInt();
                }
            case UNKNOWN_EVENT:
                {
                    qDebug() << "UNKNOWN EVENT";
                    break;
                }
        }
    }
}

void questsPlusQuest::completeQuest() {
    //Complete the quest and record current time- TODO
    completed = true;
}

void questsPlusQuest::updateQuestLog(QString newEntry)
{
    //Record new entry and when we achieved it
    questLogEntry newLogEntry;
    newLogEntry.entry = newEntry;
    newLogEntry.timestamp = QDateTime::currentDateTime();

    //Append new entry to Quest Log
    questLog.append(newLogEntry);
}


void questsPlusQuest::registerCurrentLocation(QString SceneName, double x, double y, double z)
{
    //Player_At_Location(SceneName | X | Y | Z | Radius)

    for ( int i = 0; i < BLOCKList.count(); i++)
    {
        questBLOCK * currentBLOCK = &BLOCKList[i];
        int targetTRIGGER = currentBLOCK->indexTrigger(TRIGGERType::Player_At_Location);
        while ( targetTRIGGER >= 0)
        {
            questTRIGGER * currentTRIGGER = &(currentBLOCK->TRIGGERs[targetTRIGGER]);
            float ax = currentTRIGGER->args[1].toFloat();
            float ay = currentTRIGGER->args[2].toFloat();
            float az = currentTRIGGER->args[3].toFloat();
            float ar = currentTRIGGER->args[4].toFloat();
            //qDebug() << "[QPQ][RCL]Args" << currentTRIGGER->args;

            //qDebug() << "[QPQ][RCL]PosDebug: " << ax << "," << ay << "," << az << "    " << ar << endl;

            if ( SceneName != currentTRIGGER->args[0] ) //Not the same scene
            {
                if ( currentTRIGGER->complete )
                {
                    currentTRIGGER->complete = false;
                   //qDebug() << questName << ":" << currentBLOCK->BLOCKNumber << "   LocationFailed";
                }
            }
            else if ( (currentTRIGGER->args[4].toFloat() > 0 ) && ( pow(x - currentTRIGGER->args[1].toFloat(),2) +
                     pow(y - currentTRIGGER->args[2].toFloat(),2) +
                     pow(z - currentTRIGGER->args[3].toFloat(),2) > pow( currentTRIGGER->args[4].toFloat(),2)
                     ) ) //Outside our radius
            { if ( currentTRIGGER->complete )
                { currentTRIGGER->complete = false;
                    //qDebug() << questName << ":" << currentBLOCK->BLOCKNumber << "   LocationUnCompleted";
                }
            }
            else
            {
                if ( !currentTRIGGER->complete )
                {
                    //qDebug() << questName << ":" << currentBLOCK->BLOCKNumber << "   LocationCompleted";
                    currentTRIGGER->complete = true;
                }
                if ( currentBLOCK->isComplete() )
                {
                    fireBlock(currentBLOCK->BLOCKNumber);
                }
            }


            targetTRIGGER = currentBLOCK->indexTrigger(TRIGGERType::Player_At_Location,targetTRIGGER+1);

        }
    }

}

void questsPlusQuest::registerKill(QString KillName)
{
    //qDebug() << "Kill attempting to register: " << KillName;
    //Kill(Target Name)
    for ( int i = 0; i < BLOCKList.count(); i++)
    {
        questBLOCK * currentBLOCK = &BLOCKList[i];
        //Check if we have a firable trigger that is a kill
        int targetTRIGGER = currentBLOCK->indexFirableTrigger(TRIGGERType::Kill);
        //qDebug() << "Target trigger:" << targetTRIGGER;
        while ( targetTRIGGER >= 0)
        {
            questTRIGGER * currentTRIGGER = &(currentBLOCK->TRIGGERs[targetTRIGGER]);
            QRegExp rx(currentTRIGGER->args[0]);
            //qDebug() << currentTRIGGER->args[0] << "  :  " << KillName;
            if ( rx.indexIn(KillName) >= 0 ) //Matches our target
            {
                currentTRIGGER->complete = true;
                if ( currentBLOCK->isComplete() )
                {
                    //qDebug() << "Firing Kill Block";
                    fireBlock(currentBLOCK->BLOCKNumber);
                }
                currentTRIGGER->complete = false;
            }
            targetTRIGGER = currentBLOCK->indexFirableTrigger(TRIGGERType::Kill,targetTRIGGER+1);
        }
    }
}

questsPlusQuest::questsPlusQuest(const questsPlusQuest& old) {

    //Copy constructors are good, especially since our goals self-reference their parents.
    //This copy constructor, specifically, is horrible and will be the death of me.
    //If something's crashing, look here first, something's probably not being copied/preserved right.
    StillValid = old.StillValid;
    ErrorString = old.ErrorString;
    QuestStage = old.QuestStage;
    BLOCKList = old.BLOCKList;
    newQuest = old.newQuest;
    completed = old.completed;
    questLog = old.questLog;
    questName = old.questName;
    goalList = old.goalList;
    posTRIGGER = old.posTRIGGER;
    killTRIGGER = old.killTRIGGER;
    timedTRIGGER = old.timedTRIGGER;
    chatTRIGGER = old.chatTRIGGER;
    spellCastTRIGGER = old.spellCastTRIGGER;
    lootTRIGGER = old.lootTRIGGER;

    expanded = old.expanded;


    //Initialize sound channels we're using. First initialize it all, then copy the ones we're going to use
    for ( int i = 0; i < 6; ++i)
    {
        soundChannels[i] = NULL;
        soundChannelSources[i] = "";

        if ( old.soundChannels[i] )
        {
            soundChannels[i] = new QMediaPlayer();
            soundChannelSources[i] = "BLANK";

            if ( i == 4 ) //Auto-Loop Channel
            {
                soundPlaylistLoop = new QMediaPlaylist();
                soundPlaylistLoop->setPlaybackMode(QMediaPlaylist::Loop);
                soundChannels[4]->setPlaylist(soundPlaylistLoop);
            }
            else if ( i == 5 ) //Uninterrupt Channel
            {
                soundPlaylistUninterrupt = new QMediaPlaylist();
                soundPlaylistUninterrupt->setPlaybackMode(QMediaPlaylist::Sequential);
                soundChannels[5]->setPlaylist(soundPlaylistUninterrupt);
            }

        }


    }

    //qDebug() << this << ":" << &old;
    //Update our goals parents since this has moved.
    for ( int i = 0; i < goalList.count(); i++)
    {
        goalList[i].parent = this;
    }

}


bool questsPlusQuest::loadQuest(){

    QString savePath = QDir::currentPath() + "/saves/Quest-" + questName + ".umu";
    QFile saveFile(savePath);

    //If we don't have a save file, then return false, this is a new quest.
    if ( !saveFile.exists() )
    {
        return false;
    }

    saveFile.open(QIODevice::ReadOnly);

    QTextStream saveStream(&saveFile);

    int stage = 1;
    while(!saveStream.atEnd())
    {

       //Write actual loading code here

       QString line = saveStream.readLine();
       if ( line == "====" )  //We hit a divider, advance to the next stage of loading
       {
           stage++;
       }
       else
       {
           switch(stage)
           {
           case 1: //Default Payload
            {
               QRegExp rx("([0-9]*),([0-9]*),([0-9]*),([0-9]*),([0-9]*),([0-9]*),([0-9]*),([0-9]*),([0-9]*)", Qt::CaseSensitive);

               if ( rx.indexIn(line,0) > -1 )
               {
                   completed = rx.cap(2).toInt();
                   posTRIGGER = rx.cap(3).toInt();
                   killTRIGGER = rx.cap(4).toInt();
                   timedTRIGGER = rx.cap(5).toInt();
                   chatTRIGGER = rx.cap(6).toInt();
                   spellCastTRIGGER = rx.cap(7).toInt();
                   lootTRIGGER = rx.cap(8).toInt();
                   expanded = rx.cap(9).toInt();
                   AdvanceQuestStage(rx.cap(1).toInt()); //Loading out of order so it "fixes" triggers
               }

               break;
           }
           case 2: //Journal Stage
           {
               // entry << "|?|" << timestamp << "|?|";
               QRegExp rx("(.*)\\|\\?\\|(.*)\\|\\?\\|", Qt::CaseSensitive);

               if ( rx.indexIn(line,0) > -1 )
               {
                   questLogEntry temp;
                   temp.entry = rx.cap(1);
                   temp.timestamp = QDateTime::fromString(rx.cap(2));

                   questLog.append(temp);

               }
               break;
           }
           case 3: //Goals Stage
           {
               //goalID << "|?|" goalName << "|?|" << goalStatus << "|?|" << goalTarget << "|?|";
                QRegExp rx("(.*)\\|\\?\\|(.*)\\|\\?\\|(.*)\\|\\?\\|(.*)\\|\\?\\|", Qt::CaseSensitive);

               if ( rx.indexIn(line,0) > -1 )
               {
                   questGoal temp;
                   temp.goalID = rx.cap(1);
                   temp.goalName = rx.cap(2);
                   temp.goalStatus = rx.cap(3).toInt();
                   temp.goalTarget = rx.cap(4).toInt();
                   temp.goalNumber = goalList.count();
                   temp.parent = this;

                   goalList.append(temp);

               }

               break;
           }
           case 4: //QuestBlockFiringStatus
           {

               //blockID << "|?|" << fired << "|?|" << firedDate << "|?|";
                QRegExp rx("(.*)\\|\\?\\|(.*)\\|\\?\\|(.*)\\|\\?\\|", Qt::CaseSensitive);

               if ( rx.indexIn(line,0) > -1 )
               {
                   int BLOCKNumber = rx.cap(1).toInt();
                   if ( ( BLOCKNumber >= 0 ) && ( BLOCKNumber < BLOCKList.count() ) )
                   {
                       //qDebug() << "[QPQ][LQ] at block "<< BLOCKNumber << "  :  " << rx.cap(2) << ":" << rx.cap(3);
                        BLOCKList[BLOCKNumber].fired = rx.cap(2).toInt();
                        BLOCKList[BLOCKNumber].lastFired = QDateTime::fromString(rx.cap(3));
                        //qDebug() << BLOCKList[BLOCKNumber].fired << ":" << BLOCKList[BLOCKNumber].lastFired.toString();
                   }
               }
               break;
           }
           case 5: //Should be done
               break;
           default:
               break;
           }
       }


    }
    saveFile.close();

    recalculateGoalsTriggers();
    return true;


}
int questsPlusQuest::getQuestStage() {
    return QuestStage;
}

void questsPlusQuest::saveQuest(){

    if ( !QDir(QDir::currentPath() + "/saves").exists() )
    {
        QDir().mkdir(QDir::currentPath() + "/saves");
    }

    QString savePath = QDir::currentPath() + "/saves/Quest-" + questName + ".umu";
    QFile saveFile(savePath);

    saveFile.open(QIODevice::WriteOnly|QIODevice::Truncate);

    QTextStream saveOut(&saveFile);


    //TODO: Write the actual save code here.


    //Save the quest state
    saveOut << toString() << endl;
    saveOut << "====" << endl;
    //Save the quest Log
    for ( int i = 0; i < questLog.count(); i++)
    {
        saveOut << questLog[i].toString() << endl;
    }
    saveOut << "====" << endl;
    //Save the Goals
    for ( int i = 0; i < goalList.count(); i++ )
    {
        saveOut << goalList[i].toString() << endl;
    }
    saveOut << "====" << endl;
    //Save the block firing status
    for ( int i = 0; i < BLOCKList.count(); i++)
    {
        saveOut << BLOCKList[i].toString() << endl;
    }

    saveOut.flush();
    saveFile.close();
}

QString questsPlusQuest::toString() {
    QString output = QString::number(QuestStage);
    output += ",";
    output += ( completed ? "1" : "0" );
    output += ",";
    output += ( posTRIGGER ? "1" : "0" );
    output += ",";
    output += ( killTRIGGER ? "1" : "0" );
    output += ",";
    output += ( timedTRIGGER ? "1" : "0" );
    output += ",";
    output += ( chatTRIGGER ? "1" : "0" );
    output += ",";
    output += ( spellCastTRIGGER ? "1" : "0" );
    output += ",";
    output += ( lootTRIGGER ? "1" : "0" );
    output += ",";
    output += ( expanded ? "1" : "0" );

    return output;
}

bool questsPlusQuest::isExpanded() {
    return expanded;
}

void questsPlusQuest::setExpanded(bool newStatus)
{
    //qDebug() << questName << " : " << newStatus;
    expanded = newStatus;
}

QString questsPlusQuest::getBlockDebug()
{
    QString output;
    output += "Trigger Alerts:\nKill:";
    output += ( killTRIGGER ? "true" : "false");
    output += "    POS:";
    output += ( posTRIGGER ? "true" : "false");
    output += "    Timed:";
    output += ( timedTRIGGER ? "true" : "false");
    output += "    Chat:";
    output += ( chatTRIGGER ? "true" : "false");
    output += "    SpellCast:";
    output += ( spellCastTRIGGER ? "true" : "false");
    output += "    Loot:";
    output += ( lootTRIGGER ? "true" : "false");
    output += "\n\n";
    output += "BLOCKS: \n";
    for ( int i = 0; i < BLOCKList.count(); ++i)
    {
        output += "Block #";
        output += QString::number(i);
        output += "     ";
        output += "Firable: ";
        output += ( BLOCKList[i].isFirable() ? "true" : "false");
        output += "        Complete: ";
        output += ( BLOCKList[i].isComplete() ? "true" : "false");
        output += "        Fired: ";
        output += ( BLOCKList[i].fired ? "true" : "false");
        output += "  at: ";
        output += BLOCKList[i].lastFired.toString() ;

        output += "\nTRIGGERs:\n";

        for ( int j = 0; j < BLOCKList[i].TRIGGERs.count(); ++j)
        {
            output += questBLOCK::getTRIGGERType(BLOCKList[i].TRIGGERs[j].type) + ":";
            output += ( BLOCKList[i].TRIGGERs[j].complete ? "completed" : "incomplete" );
            output += ":";
            output += QString::number(BLOCKList[i].TRIGGERs[j].args.count()) + "     ARGS:  ";
            for ( int k = 0; k < BLOCKList[i].TRIGGERs[j].args.count(); ++k)
            {
                output += BLOCKList[i].TRIGGERs[j].args[k];
                output += "   ||   ";
            }
            output += "\n";
        }
        output += "\nEVENTS:\n";
        for ( int j = 0; j < BLOCKList[i].EVENTs.count(); ++j)
        {
            output += questBLOCK::getEVENTType(BLOCKList[i].EVENTs[j].type) + "  ArgCount:" + QString::number(BLOCKList[i].EVENTs[j].args.count()) + "   ARGS:   ";
            for ( int k = 0; k < BLOCKList[i].EVENTs[j].args.count(); ++k)
            {
                output += BLOCKList[i].EVENTs[j].args[k];
                output += "   ||   ";
            }
            output += "\n";
        }
        output += "\n";
    }

    return output;
}

bool questsPlusQuest::createSoundChannel(int channel)
{
    /*qDebug() << "Existing Channels:";
    for ( int i = 0; i < 6; ++i)
    {
        if ( soundChannels[channel] )
        {
            qDebug() << i;
        }
    }
    qDebug() << "Creating sound channel: " << channel;
    */
    if ( ( channel < 0 ) || (channel > 5 ) ) //Out of bounds
    {
        qDebug() << "WTF GUYS, channels 0-5 ONLY for sound. You shouldn't ever see this";
        return false;
    }

    if ( soundChannelSources[channel] == "" ) //Blank channel, uninitialized
    {
        //Initialize the channel
        soundChannels[channel] = new QMediaPlayer();
        soundChannelSources[channel] = "Blank";

        if ( channel == 4 )
        {
            soundPlaylistLoop = new QMediaPlaylist();
            soundPlaylistLoop->setPlaybackMode(QMediaPlaylist::Loop);
            soundChannels[4]->setPlaylist(soundPlaylistLoop);
        }
        if ( channel == 5 )
        {
            soundPlaylistUninterrupt = new QMediaPlaylist();
            soundPlaylistUninterrupt->setPlaybackMode(QMediaPlaylist::Sequential);
            soundChannels[5]->setPlaylist(soundPlaylistUninterrupt);
        }
    }

    return true;
}

void questsPlusQuest::playQuestSound(QString file, int channel)
{
    QString filepath = QDir::currentPath() + "/quests/" + questName + "/" + file;

    //qDebug() << "Playing sound: " << file << "    on channel " << channel;
    if ( ( channel < 0 ) || (channel > 5 ) ) //Out of bounds
    {
        qDebug() << "WTF GUYS, channels 0-5 ONLY for sound. You shouldn't ever see this";
        return;
    }



    if ( soundChannelSources[channel] != file )//Not the current file in the channel, load it up
    {
        if ( channel < 4 ) //Channels 4 and 5 have special rules
        {
            soundChannels[channel]->setMedia(QUrl::fromLocalFile(QFileInfo(filepath).absoluteFilePath()));
            soundChannelSources[channel] = file;
        }
        else if ( channel == 4 ) //Channel 4 - Looped
        {
            soundPlaylistLoop->clear();
            soundPlaylistLoop->addMedia(QUrl::fromLocalFile(QFileInfo(filepath).absoluteFilePath()));
            soundPlaylistLoop->setCurrentIndex(0);


        }
        else //Must be channel 5
        {
            if ( soundChannels[channel]->state() != QMediaPlayer::PlayingState )
            {
                //If we aren't playing, clear the playlist;
                soundPlaylistUninterrupt->clear();
            }
            soundPlaylistUninterrupt->addMedia(QUrl::fromLocalFile(QFileInfo(filepath).absoluteFilePath()));
        }

    }

    //Now that all that's done, play the file!
    if ( channel < 4 )
    {
        soundChannels[channel]->setPosition(0);
        soundChannels[channel]->play();
    }
    else if ( channel == 4 ) //Channel 4 - Looped
    {
        soundChannels[channel]->play();


    }
    else //Must be channel 5, queued
    {
        //Only play if we're not playing, to not screw up playlist.
        if ( soundChannels[channel]->state() != QMediaPlayer::PlayingState )
        {
            soundChannels[channel]->play();
        }

    }
}
void questsPlusQuest::registerChat(QStringList chatStrings, ChatType::ChatType chatType)
{
    //qDebug() << "Chat attempting to register: " << chatStrings[1] << " - " << chatStrings[2] << " - " << chatStrings[3];

    QString temp;

    TRIGGERType tempType = TRIGGERType::UNKNOWN_TRIGGER; //Just in case
    switch (chatType)
    {
        case ChatType::Chat:
        {
            if ( chatStrings[2].length() > 0 ) //There's a channel in this chat, may even be a whisper
            {
                tempType = TRIGGERType::Chat_Channel;
            }
            else
            {
                tempType = TRIGGERType::Chat;
            }
            break;
        }
        case ChatType::Emote:
        {
            tempType = TRIGGERType::Emote;
            break;
        }
        case ChatType::Whisper:
        {
            tempType = TRIGGERType::Chat_Channel;
            break;
        }
        default:
        {
            break;
        }
    }

    for ( int i = 0; i < BLOCKList.count(); i++)
    {
        questBLOCK * currentBLOCK = &BLOCKList[i];

        int targetTRIGGER = currentBLOCK->indexFirableTrigger(tempType);
        while ( targetTRIGGER >= 0)
        {
            questTRIGGER * currentTRIGGER = &(currentBLOCK->TRIGGERs[targetTRIGGER]);

            bool GoodToFire = false;
            switch (tempType)
                {
                case TRIGGERType::Chat:
                {
                    QRegExp ChatString(currentTRIGGER->args[0],Qt::CaseInsensitive);
                    QRegExp Source(currentTRIGGER->args[1],Qt::CaseInsensitive);
                    if (( ChatString.indexIn(chatStrings[3]) >= 0 )
                            && ( Source.indexIn(chatStrings[1]) >= 0 ) ) //Matches our spell and source targets
                    {
                        GoodToFire = true;
                    }
                    break;
                }
                case TRIGGERType::Chat_Channel:
                {
                    QRegExp ChatString(currentTRIGGER->args[0],Qt::CaseInsensitive);
                    QRegExp Source(currentTRIGGER->args[1],Qt::CaseInsensitive);
                    QRegExp Channel(currentTRIGGER->args[2],Qt::CaseInsensitive);
                    if (( ChatString.indexIn(chatStrings[3]) >= 0 )
                            && ( Source.indexIn(chatStrings[1]) >= 0 )
                            && ( Channel.indexIn(chatStrings[2]) >= 0 ) ) //Matches our spell and source targets
                    {
                        GoodToFire = true;
                    }
                    break;
                }
                case TRIGGERType::Emote:
                {
                    //To Implement
                    break;
                }
                default:
                {
                    break;
                }
            }
            if ( GoodToFire )
            {
                currentTRIGGER->complete = true;
                if ( currentBLOCK->isComplete() )
                {
                    //qDebug() << "Firing " << tempType <<  " Block";
                    fireBlock(currentBLOCK->BLOCKNumber);
                }
                currentTRIGGER->complete = false;
            }
            targetTRIGGER = currentBLOCK->indexFirableTrigger(tempType,targetTRIGGER+1);
        }
    }
}

void questsPlusQuest::registerSpellCast(QStringList chatStrings)
{
    //chatStrings[0] is the full chat
    //chatStrings[1] source casting the spell
    //chatStrings[2] is the spell being cast
//    qDebug() << "Spell cast attempting to register: " << chatStrings[1] << " casting " << chatStrings[2];

    for ( int i = 0; i < BLOCKList.count(); i++)
    {
        questBLOCK * currentBLOCK = &BLOCKList[i];
        //Check if we have a firable trigger that is a spell_cast
        int targetTRIGGER = currentBLOCK->indexFirableTrigger(TRIGGERType::Spell_Cast);
        while ( targetTRIGGER >= 0)
        {
            questTRIGGER * currentTRIGGER = &(currentBLOCK->TRIGGERs[targetTRIGGER]);

            QRegExp wordsOfPower(currentTRIGGER->args[0],Qt::CaseInsensitive);
            QRegExp caster(currentTRIGGER->args[1],Qt::CaseInsensitive);
            if (( wordsOfPower.indexIn(chatStrings[2]) >= 0 )
                    && ( caster.indexIn(chatStrings[1]) >= 0 ) ) //Matches our spell and source targets
            {
                currentTRIGGER->complete = true;
                if ( currentBLOCK->isComplete() )
                {
                    //qDebug() << "Firing Spell_Cast Block";
                    fireBlock(currentBLOCK->BLOCKNumber);
                }
                currentTRIGGER->complete = false;
            }
            targetTRIGGER = currentBLOCK->indexFirableTrigger(TRIGGERType::Spell_Cast,targetTRIGGER+1);
        }
    }

}


void questsPlusQuest::registerLoot(QString item, int amount)
{
    //qDebug() << "[QPQ][registerLoot]" << item << ":" << amount;
    for ( int i = 0; i < BLOCKList.count(); i++)
    {
        questBLOCK * currentBLOCK = &BLOCKList[i];
        //Check if we have a firable trigger that is a spell_cast
        int targetTRIGGER = currentBLOCK->indexFirableTrigger(TRIGGERType::Loot);
        while ( targetTRIGGER >= 0)
        {
            questTRIGGER * currentTRIGGER = &(currentBLOCK->TRIGGERs[targetTRIGGER]);

            QRegExp itemTarget(currentTRIGGER->args[0],Qt::CaseInsensitive);
            if ( itemTarget.indexIn(item) >= 0 )
            {
                currentTRIGGER->complete = true;
                if ( currentBLOCK->isComplete() )
                {
                    fireBlock(currentBLOCK->BLOCKNumber);
                    if ( currentBLOCK->repeatable ) //Fire once per amount when repeatable
                    {
                        for ( int j = 1; j < amount; ++j)
                        {
                            fireBlock(currentBLOCK->BLOCKNumber);
                        }
                    }

                }
                currentTRIGGER->complete = false;
            }
            targetTRIGGER = currentBLOCK->indexFirableTrigger(TRIGGERType::Loot,targetTRIGGER+1);
        }
    }


}

QList<questLogEntry> questsPlusQuest::getQuestLog()
{
    return questLog;
}


void questsPlusQuest::recalculateGoalsTriggers()
{
    int totalCompleted = 0;
    QStringList completedGoals;
    for ( int i = 0; i < goalList.count(); ++i)
    {
        if ( goalList[i].isComplete() )
        {
            ++totalCompleted;
            completedGoals.append(goalList[i].goalID);
        }
    }


    //First do the Goals_Complete TRIGGER, which checks for a sum.
    for ( int i = 0; i < BLOCKList.count(); ++i)
    {
        //First do the Goals_Complete trigger, then do the Goal_Complete trigger
        questBLOCK * currentBLOCK = &BLOCKList[i];
        int targetTRIGGER = currentBLOCK->indexTrigger(TRIGGERType::Goals_Complete);
        while ( targetTRIGGER >= 0 )
        {
              questTRIGGER * currentTRIGGER = &(currentBLOCK->TRIGGERs[targetTRIGGER]);

             if ( totalCompleted >= currentTRIGGER->args[0].toInt() )
             {
                 if ( !currentTRIGGER->complete )
                 {
                     currentTRIGGER->complete = true;
                     if ( ( currentBLOCK->isComplete() ) && ( currentBLOCK->isFirable() ) )
                     {
                         fireBlock(currentBLOCK->BLOCKNumber);
                     }
                 }
             }
             else
             {
                 currentTRIGGER->complete = false;
             }
             targetTRIGGER = currentBLOCK->indexTrigger(TRIGGERType::Goals_Complete,targetTRIGGER+1);
        }

        //Check for completion of specific goal
        targetTRIGGER = currentBLOCK->indexTrigger(TRIGGERType::Goal_Complete);
        while ( targetTRIGGER >= 0 )
        {
              questTRIGGER * currentTRIGGER = &(currentBLOCK->TRIGGERs[targetTRIGGER]);
             if ( completedGoals.indexOf(currentTRIGGER->args[0]) > -1 ) //This is in our completed goals list
             {
                 if ( !currentTRIGGER->complete )
                 {
                     currentTRIGGER->complete = true;
                     if ( ( currentBLOCK->isComplete() ) && ( currentBLOCK->isFirable() ) )
                     {
                         fireBlock(currentBLOCK->BLOCKNumber);
                     }
                 }
                 else
                 {
                     currentTRIGGER->complete = false;
                 }
             }
             targetTRIGGER = currentBLOCK->indexTrigger(TRIGGERType::Goal_Complete,targetTRIGGER+1);
        }

        //Check for marking any Goal_Not_Complete triggers as failed
        targetTRIGGER = currentBLOCK->indexTrigger(TRIGGERType::Goal_Not_Complete);
        while ( targetTRIGGER >= 0 )
        {
              questTRIGGER * currentTRIGGER = &(currentBLOCK->TRIGGERs[targetTRIGGER]);
             if ( completedGoals.indexOf(currentTRIGGER->args[0]) > -1 ) //This is in our completed goals list
             {
                 currentTRIGGER->complete = false; //Goal is complete, so now the not complete trigger is false
             }
             else
             {
                 currentTRIGGER->complete = true;
             }
             targetTRIGGER = currentBLOCK->indexTrigger(TRIGGERType::Goal_Not_Complete,targetTRIGGER+1);
        }

    }

}
