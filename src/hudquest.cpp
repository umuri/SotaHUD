#include "hudquest.h"
#include "ui_hudquest.h"
#include <QMessageBox>


hudQuest::hudQuest(QWidget *parent, questsPlusController * inController) :
    hudWindow(parent),
    ui(new Ui::hudQuest)
{
    ui->setupUi(this);

    questController = inController;

    questModel = new hudQuestsModel(this, questController);



    ui->treeQuestList->setModel(questModel);

    connect(ui->treeQuestList, SIGNAL(doubleClicked(QModelIndex)), questModel, SLOT(indexDoubleClicked(QModelIndex)));
    connect(ui->treeQuestList, SIGNAL(expanded(QModelIndex)), questModel, SLOT(qexpanded(QModelIndex)));
    connect(ui->treeQuestList, SIGNAL(collapsed(QModelIndex)), questModel, SLOT(qcollapsed(QModelIndex)));
    //This must be at the end of every HudWindow constructor in order to properly setup transparent backgrounds.
   finishSetup();
}


void hudQuest::dataChanged()
{
    questModel->updateData();

    //Check expanded status and re-expand anything that needs it.
    for ( int i = 0; i < ui->treeQuestList->model()->rowCount() ;++i  )
    {
        QModelIndex sourceIndex = ui->treeQuestList->model()->index( i, 0 );


        if ( questController->questAt(i)->isExpanded() )
        {
            if ( !ui->treeQuestList->isExpanded(sourceIndex) )
            {

                ui->treeQuestList->setExpanded( sourceIndex, true );
            }
        }
        else
        {
            if ( ui->treeQuestList->isExpanded(sourceIndex) )
            {
                ui->treeQuestList->setExpanded( sourceIndex, false );
            }
        }
    }

}

hudQuest::~hudQuest()
{
    delete ui;
}


void hudQuest::help(QPoint /*pos*/)
{


    QString message;
    message = "Quest Window\n";
    message = message + "This window is still under development\n";
    message = message + "Thanks to all the livestreamers watching!\n";

    QMessageBox about(QMessageBox::NoIcon, "Quests Help", message,
                                  QMessageBox::Ok);

        about.setTextInteractionFlags(Qt::TextSelectableByMouse);


    if ( isOnTop() )
    {
     about.setWindowFlags(about.windowFlags()|Qt::WindowStaysOnTopHint);
    }

    about.exec();


}

