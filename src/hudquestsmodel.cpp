#include "hudquestsmodel.h"
#include <QDebug>
#include <QMessageBox>

hudQuestsModel::hudQuestsModel(QObject *parent, questsPlusController * inController)
    : QAbstractItemModel(parent)
{
    questController = inController;
}

QVariant hudQuestsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal) {
            switch (section)
            {
            case 0:
                return QString("Quests");
            case 1:
                return QString("Status");
            }
        }
    }
    return QVariant();
}

bool hudQuestsModel::hasChildren(const QModelIndex &parent)
{
    if ( !parent.isValid() )
    {
        return true;
    }
    else
    {
           return false;
    }
}


QModelIndex hudQuestsModel::index(int row, int column, const QModelIndex &parent) const
{
    //qDebug() << row << "," << column << ":" << parent.internalId() << (int)parent.internalId() << ":" << parent.internalPointer();
    if (!hasIndex(row, column, parent))
    {
        //qDebug() << "INVALID";
           return QModelIndex();
    }
    if ( !parent.isValid() ) //We're at the root, so display quests
    {
        //qDebug() << "Root at" << row << ":" << column;
        return createIndex(row,column,questController->questAt(row));
    }
    if ( (void *)parent.internalPointer() == (void *)&questController )
    {
        //qDebug() << "Root2 at" << row << ":" << column;
        return createIndex(row,column,questController->questAt(row));
    }
    //Check if our parent is a quest
    int questID = questController->rowAt((questsPlusQuest* )parent.internalPointer());
    if ( questID >= 0 ) //This is a quest, so display the goals
    {


        questsPlusQuest * currentQuest = questController->questAt(questID);
        //qDebug() << questID << " Quest looking for goal:" << row << "," << column;
        //qDebug() << currentQuest->getQuestName() << currentQuest->getGoal(row) << (void *)currentQuest->getGoalPtr(row);
        return createIndex(row,column,(void *)currentQuest->getGoalPtr(row));
    }
    else
    {
        return QModelIndex();
    }
}

QModelIndex hudQuestsModel::parent(const QModelIndex &index) const
{
    //qDebug() << index.row() << "," << index.column() << "|" << index.internalPointer() << "|" << index.internalId();

    if ( !index.isValid() )
    {
        return QModelIndex();
    }
    else if ( !index.internalPointer() )
    {
        //qDebug() << "shouldn't reach this: " << index.row() << ":" << index.column();
        return QModelIndex();
    }
    else if ( !index.isValid() )
    {
        //qDebug() << "WTF";
        return QModelIndex();
    }

    int questID = questController->rowAt((questsPlusQuest* )index.internalPointer());
    //qDebug() << questID;
    if ( questID >= 0 ) //This is a quest!
    {
        return QModelIndex(); //Since this is a quest, we are at root level, return invalid.
        //return createIndex(questID,0);
    }

    //We're not at root level, this should have a quest for a parent.
    //qDebug() << "Goal checking";
       //This should be a goal as the parent is not a quest and is valid
        questGoal * currentIndex = static_cast<questGoal*>(index.internalPointer());
        int targetRow = currentIndex->goalNumber;
        if ( targetRow >= 0 )
        {   
            return createIndex(targetRow, 0, currentIndex->parent);
        }
        else
        {
            return QModelIndex();
        }

     qDebug() << "How did we get here?" << index.row() << ":" << index.column();



    return QModelIndex();


    //If item's parent is the root, return an index for where it is from the roo
    //If items parent is a quest, reutrn an index to where it is from the root
    //If items parent is a goal, that shouldn't happen


}

int hudQuestsModel::rowCount(const QModelIndex &parent) const
{
    if (parent.column() > 0)
           return 0;

    if (!parent.isValid()) //Top Row
    {
        //We have as many rows as we do quests, for now
            return questController->count();
    }

    int questID = questController->rowAt((questsPlusQuest* )parent.internalPointer());
    //qDebug() << questID;
    if ( questID >= 0 ) //This is a quest!
    {

        questsPlusQuest * currentQuest = static_cast<questsPlusQuest*>(parent.internalPointer());
        //questsPlusQuest *debugQuest = questController->questAt(questID);
        //qDebug() << currentQuest->getGoalsCount() << ":" << debugQuest->getGoalsCount();
        //qDebug() << "RowCount Goal: " << questID << " with rows: " << parent.row() << "," << parent.column() << "||" << currentQuest->getGoalsCount() << "    " << currentQuest->getQuestName();
        return currentQuest->getGoalsCount();
    }
    else
    {
        return 0;
    }

}

int hudQuestsModel::columnCount(const QModelIndex &parent) const
{
    //We have two columns for now
    return 2;

    if (!parent.isValid()) //Top row, 2 columns
    {
        return 2;
    }

}

void hudQuestsModel::updateData()
{
    emit dataChanged(createIndex(0,0), createIndex(questController->count(),1));
    emit layoutChanged();
}

QVariant hudQuestsModel::data(const QModelIndex &index, int role) const
{

    if (role == Qt::DisplayRole)
    {
        //qDebug() << index.row() << ":" << index.column() << "|" << index.internalPointer();
       if ( !index.parent().isValid() )
       {
           //qDebug() << "Root";
        switch(index.column())
            {
            case 0: //Column 1 - Quest or Subquest name
                return questController->questNameAt(index.row());
                break;
            case 1: //Column 2 - Status
                return questController->questStatusAt(index.row());
                break;
             default:
                return "Whatever";
                break;
            }
        }
        else //Child node!
        {
           questGoal * currentGoal = (questGoal *)index.internalPointer();
           switch (index.column() )
           {
               case 0:
                   return currentGoal->getGoalStatus();
                  break;
               case 1:
                    return currentGoal->goalName;
                   break;
               default:
                   return "Unknown";
                   break;
               }

        }
    }
    if ( role == Qt::FontRole )
    {
        if ( !index.parent().isValid() )
        {

        }
        else //Goal node
        {
            switch (index.column() )
            {
                case 0:
                {
                    QFont font("Monospace");
                    font.setStyleHint(QFont::TypeWriter);
                    return font;
                    break;
                }
                case 1:
                    break;
                default:
                    break;
            }
        }
    }

    return QVariant();
}


Qt::ItemFlags hudQuestsModel::flags(const QModelIndex & index) const
{
    if ( index.column() == 2) //Will never be 2, leaving this in for reference
    {
        return Qt::ItemIsSelectable |  Qt::ItemIsEditable | Qt::ItemIsEnabled ;
    }
        else return Qt::ItemIsEnabled;
}

void hudQuestsModel::indexDoubleClicked(const QModelIndex & index )
{
    bool DEBUG = true;
    QString message;

    if ( !index.isValid() )
    {
        return;
    }

    //Determine debug status:

    if ( !index.parent().isValid() )
    {
        DEBUG = questController->questAt(index.row())->DEBUG;
    }
    else //Child node.
    {
         questGoal * currentGoal = (questGoal *)index.internalPointer();
         DEBUG = currentGoal->parent->DEBUG;
    }

    if ( DEBUG )
    {
        if ( !index.parent().isValid() )
        {
            if ( index.column() == 1 )
            {
                questsPlusQuest * clickedQuest = questController->questAt(index.row());
                if ( !clickedQuest->isValid() )
                {
                    return;
                }
                message = "Quest Debug: " + clickedQuest->getQuestName() + "\n";
                message += "Current Stage: " + QString::number(clickedQuest->getQuestStage()) + "\n\n";
                message += "Current Goal Count: " + QString::number(clickedQuest->getGoalsCount()) + "\n";
                for ( int i = 0; i < clickedQuest->getGoalsCount(); i++ )
                {
                    message += QString::number(i) + " ( " + clickedQuest->getGoalStatus(i) + " ) " +  "        " + clickedQuest->getGoalName(i) + "\n";
                }
                message += "\n\n";
                message += "BLOCKS: \n";
                message += clickedQuest->getBlockDebug();


            }
            else
            {
                return;
            }
         }
         else //Child node!
         {
            questGoal * currentGoal = (questGoal *)index.internalPointer();

            message = "Goal Debug: " + currentGoal->goalID + "\n";
            message += "Goal Name: " + currentGoal->goalName + "\n";
            message += "Goal Status: " + QString::number(currentGoal->goalStatus) + "\n";
            message += "Goal Target: " + QString::number(currentGoal->goalTarget) + "\n";
            message += "Goal Display: " + currentGoal->getGoalStatus() + "\n";

         }
        QMessageBox debugBox(QMessageBox::NoIcon, "Quest Debug Info", message,
                                        QMessageBox::Ok);
        debugBox.setTextInteractionFlags(Qt::TextSelectableByMouse);

        debugBox.setWindowFlags(debugBox.windowFlags()|Qt::WindowStaysOnTopHint);

        debugBox.exec();
    }
    else //Not debug
    {
        if ( !index.parent().isValid() )
        {

            questController->currentJournal.setQuest(questController->questAt(index.row()));
            questController->currentJournal.display();
        }
        else //Child node.
        {
             questGoal * currentGoal = (questGoal *)index.internalPointer();
             questController->currentJournal.setQuest(currentGoal->parent);
             questController->currentJournal.display();
        }

    }





}

void hudQuestsModel::qexpanded(const QModelIndex & index )
{
    if ( !index.parent().isValid() )
    {
        questsPlusQuest * clickedQuest = questController->questAt(index.row());
        clickedQuest->setExpanded(true);
     }
    return;
}

void hudQuestsModel::qcollapsed(const QModelIndex & index )
{
    if ( !index.parent().isValid() )
    {
        questsPlusQuest * clickedQuest = questController->questAt(index.row());
        clickedQuest->setExpanded(false);
     }
    return;
}
