#include "hudquestjournal.h"
#include "ui_hudquestjournal.h"
#include <QResizeEvent>


hudQuestJournal::hudQuestJournal(QWidget *parent) :
    hudWindow(parent),
    ui(new Ui::hudQuestJournal)
{
    ui->setupUi(this);

    backgroundImage = new QPixmap(":/port_Book_UI_Simple_Page_Border.png");

    QPixmap tempPix = backgroundImage->scaled(this->width(), this->height());
    lblBackground = new QLabel(this);
    lblJournalText = new QLabel(this);
    lblJournalText->setWordWrap(true);
    lblBackground->setGeometry(0,0,this->width(),this->height());
    lblBackground->setPixmap(tempPix);



    //This must be at the end of every HudWindow constructor in order to properly setup transparent backgrounds.
   finishSetup();

   //setStyleSheet("background-image: url(:/port_Book_UI_Simple_Page_Border.png);");
}

void hudQuestJournal::setQuest(questsPlusQuest * questToSet)
{
    //Create the stuff
    //Add the stuff
    journal = "";

    QList<questLogEntry> currentLog = questToSet->getQuestLog();
    if ( currentLog.count() > 0 )
    {
        for ( int i = 0; i < currentLog.count(); i++)
        {
            journal += "~~PGBREAK~~";
            journal += currentLog[i].timestamp.toString() + "<br>";
            journal += currentLog[i].entry;
        }
        journal += "~~PGBREAK~~";
    }
    else //Nothing in the journal, quest not started yet
    {
        journal = "~~PGBREAK~~Quest Not Started~~PGBREAK~~";
    }
    QString pgbreak = "<br><img width=\"" + QString::number(this->width()-20) + "\" src=\":/port_Book_UI_Filagree_Ornament_02\"><br>";




    QString journalTemp = journal;
    journalTemp = journalTemp.replace("~~PGBREAK~~",pgbreak);
    lblJournalText->setText(journalTemp);

}

void hudQuestJournal::keyPressEvent(QKeyEvent *e) {
    if(e->key() == Qt::Key_Escape)
      {
        this->hide();
    }
}

void hudQuestJournal::display()
{
    show();
    setFocus();
}


void hudQuestJournal::resizeEvent(QResizeEvent * event)
{
    hudWindow::resizeEvent(event);



    lblBackground->setGeometry(0,0,event->size().width(),event->size().height());
    QPixmap tempPix(":/port_Book_UI_Simple_Page_Border.png");
    tempPix = tempPix.scaled(event->size().width(),event->size().height());
    lblBackground->setPixmap(tempPix);
    lblBackground->repaint();
    if (( event->size().width() > 20 ) && ( event->size().height() > 20 ) )
    {
        QString pgbreak = "<br><img width=\"" + QString::number(event->size().width()-20) + "\" src=\":/port_Book_UI_Filagree_Ornament_02\"><br>";
        QString journalTemp = journal;
        journalTemp = journalTemp.replace("~~PGBREAK~~",pgbreak);
        lblJournalText->setText(journalTemp);
        lblJournalText->setGeometry(20,20,event->size().width()-20,event->size().height()-20);
    }
}

hudQuestJournal::~hudQuestJournal()
{
    delete ui;
}
