#include "hudclock.h"
#include "ui_hudclock.h"
#include "QMenu"
#include "QDir"
#include <QMessageBox>
#include <QXmlStreamReader>
#include <QTimeZone>

hudClock::hudClock(QWidget *parent, Config * tempConfig, notificationManager * tempPopup) :
    hudWindow(parent),
    ui(new Ui::hudClock)
{
    showCloseButton = false;

    appConfig = tempConfig;
    ui->setupUi(this);

    timerID = startTimer(1000);
    systemLocale = QLocale::system();

    //addEvent("Bacon time!" , QDateTime::currentDateTime().addSecs(60));
    //addEvent("Bacon timex2!" , QDateTime::currentDateTime().addSecs(260));

    //connect(addButton, &QPushButton::clicked, this, addButtonClicked);
    connect(&removeButtonGroup, SIGNAL(buttonClicked(int)),  this, SLOT(removeButtonGroupClicked(int))  );
    connect(&plusButtonGroup, SIGNAL(buttonClicked(int)),  this, SLOT(plusButtonGroupClicked(int))  );
    connect(&infoButtonGroup, SIGNAL(buttonClicked(int)),  this, SLOT(infoButtonGroupClicked(int))  );

    ui->gridLayout->setColumnMinimumWidth(0,25);
    ui->gridLayout->setColumnMinimumWidth(1,10);
    ui->gridLayout->setColumnMinimumWidth(2,10);

    lastTimerText = "Timer";
    lastTimerTime = "5:00";

    alarmSound = new QSoundEffect();
    alarmSound->setSource(QUrl::fromLocalFile(":/clockAlarm.wav"));


    QAction * addTimerAction = new QAction("Add Timer");

    settingsActionList.push_back(addTimerAction);
    connect(addTimerAction, &QAction::triggered, this, &hudClock::addTimerActionClicked);

    timeZoneOffset =  ((QDateTime::currentDateTime().timeZone().offsetFromUtc(QDateTime::currentDateTime()) )/3600);
    popupManager = tempPopup;

    loadEvents();
    //This must be at the end of every HudWindow constructor in order to properly setup transparent backgrounds.
   finishSetup();
}





/*
void hudClock::resizeEvent( QResizeEvent* z ) {
    hudWindow::resizeEvent(z);
    int widthPlacement = width() - 15;
}
*/

hudClock::~hudClock()
{
    delete ui;
}

void hudClock::timerEvent(QTimerEvent * /*e*/)
{

    //Add the clock time
    ui->lblClock->setText(QDateTime::currentDateTime().toString(systemLocale.timeFormat()));

    for ( int i = 0; i < eventList.length(); i++ )
    {
        long secsUntil = QDateTime::currentDateTime().secsTo(eventList[i].eventTime);
        if ( ( secsUntil < 1 ) && ( eventList[i].soundAlarm ) )
        {

            alarmSound->play();
            //Do alert here
            eventList[i].soundAlarm = false;
        }
        if ( secsUntil < ( 60 * 5 ) && eventList[i].fiveMinuteWarning ) //5 minute warning
        {
            popupManager->addNotification("5 minute warning", eventList[i].eventName + " is occuring in 5 minutes");
            eventLabelList[i]->setStyleSheet("QLabel { background-color : rgba(31,31,31,0.5); color : red; border: 1px solid rgb(214,171,83); }");
            eventList[i].fiveMinuteWarning=false;
            eventList[i].fifteenMinuteWarning=false;
            eventList[i].thirtyMinuteWarning=false;
        }
        else if ( secsUntil < (60*15 ) && eventList[i].fifteenMinuteWarning ) //15 minute warning
        {
            popupManager->addNotification("15 minute warning", eventList[i].eventName + " is occuring in 15 minutes");
            eventLabelList[i]->setStyleSheet("QLabel { background-color : rgba(31,31,31,0.5); color : yellow; border: 1px solid rgb(214,171,83); }");
            eventList[i].fifteenMinuteWarning=false;
            eventList[i].thirtyMinuteWarning=false;
        }
        else if ( secsUntil < (60*30 ) && eventList[i].thirtyMinuteWarning ) //30 minute warning
        {
            popupManager->addNotification("30 minute warning", eventList[i].eventName + " is occuring in 30 minutes");
            eventLabelList[i]->setStyleSheet("QLabel { background-color : rgba(31,31,31,0.5); color : green; border: 1px solid rgb(214,171,83); }");
            eventList[i].thirtyMinuteWarning=false;
        }

        QString timeUntil = "(" + QString::number(secsUntil/60/60) + ":" + QString::number((secsUntil/60)%60)+ ":" + QString::number(secsUntil%60) + ")";

        eventLabelList[i]->setText(eventList[i].eventName + timeUntil);
    }

}

void hudClock::addTimer(QString timerText, int secs )
{

    Event newEvent;
    newEvent.eventName = timerText;
    newEvent.timerSecs = secs;
    newEvent.eventTime = QDateTime::currentDateTime().addSecs(secs);
    addEvent(newEvent);

}

void hudClock::addEvent(Event newEvent)
{
    if (( QDateTime::currentDateTime().secsTo(newEvent.eventTime) <  0 ) //It already happened
        || (QDateTime::currentDateTime().secsTo(newEvent.eventTime) > ( 60 * 60 * appConfig->ShowEventsHappeningInXHours ) ) ) //It is further out than our setting, default 2 days.
    {
        return;
    }


    //Calculate Timezone Offset
    //qDebug() << timeZoneOffset << ":" << newEvent.eventTimeZone;
    if ( newEvent.timerSecs <= 0 ) //Its not a timer
    {
        int offset = timeZoneOffset - newEvent.eventTimeZone;
        if ( offset != 0 )//We're not in our normal timezone
        {
            //qDebug() << offset << ":" << offset * 3600;
            newEvent.eventTime = newEvent.eventTime.addSecs(offset * 3600);
        }
    }
    //Assign our button count to allow the reset/remove buttons to work
    buttonCount++;
    newEvent.buttonID = buttonCount;

    eventList.append(newEvent);

    QLabel * newLabel = new QLabel(this);
    newLabel->setAlignment(Qt::AlignHCenter);


    long secsUntil = QDateTime::currentDateTime().secsTo(newEvent.eventTime);
    QString timeUntil = "(" + QString::number(secsUntil/60/60) + ":" + QString::number((secsUntil/60)%60)+ ":" + QString::number(secsUntil%60) + ")";

    newLabel->setText(newEvent.eventName + timeUntil);

    eventLabelList.append(newLabel);



    QString imagePath =  QDir::currentPath() +"/icons/";

    QPushButton * button = new QPushButton();
    button->setStyleSheet("border:0px;");
    QPixmap closePixmap(imagePath + "close.png");
    QIcon closeButtonIcon(closePixmap);
    button->setFixedSize(13,15);
    button->setIcon(closeButtonIcon);
    button->setIconSize(closePixmap.rect().size());


    removeButtonGroup.addButton(button,buttonCount);

    ui->gridLayout->addWidget(newLabel,ui->gridLayout->rowCount(),0);
    ui->gridLayout->addWidget(button,ui->gridLayout->rowCount()-1,1);

    if ( newEvent.timerSecs > 0 ) //Timer
    {
    QString plusImagePath =  QDir::currentPath() +"/icons/";

    QPushButton * plusButton = new QPushButton();
    plusButton->setStyleSheet("border:0px;");
    QPixmap plusPixmap(plusImagePath + "plus.png");
    QIcon plusButtonIcon(plusPixmap);
    plusButton->setFixedSize(13,15);
    plusButton->setIcon(plusButtonIcon);
    plusButton->setIconSize(plusPixmap.rect().size());

    plusButtonGroup.addButton(plusButton,buttonCount);
    ui->gridLayout->addWidget(plusButton,ui->gridLayout->rowCount()-1,2);

    }
    else //Event
    {
        QString infoImagePath =  QDir::currentPath() +"/icons/";

        QPushButton * infoButton = new QPushButton();
        infoButton->setStyleSheet("border:0px;");
        QPixmap infoPixmap(infoImagePath + "qmark.png");
        QIcon infoButtonIcon(infoPixmap);
        infoButton->setFixedSize(13,15);
        infoButton->setIcon(infoButtonIcon);
        infoButton->setIconSize(infoPixmap.rect().size());

        infoButtonGroup.addButton(infoButton,buttonCount);
        ui->gridLayout->addWidget(infoButton,ui->gridLayout->rowCount()-1,2);



    }




    adjustSize();

}



void hudClock::loadEvents()
{
    QString pathTemp;
    pathTemp = QDir::currentPath() + "/events/";

    QDir mapFolder(pathTemp);

    for ( unsigned int i = 0; i < mapFolder.count(); i++)
    {

        QRegExp rx("(.*).xml", Qt::CaseInsensitive);

        if ( rx.indexIn(mapFolder.entryList()[i],0) >= 0)
        {
            processEventsFile(mapFolder.entryList()[i]);
        }

    }
}

void hudClock::processEventsFile(QString File )
{
    //qDebug() << File;


    QString pathTemp;
    pathTemp = QDir::currentPath() + "/events/";

    QFile xmlFile(pathTemp + "/" + File);

    if (!xmlFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
                        return;
    }
    QXmlStreamReader xmlReader(&xmlFile);

    Event tempEvent;
    bool filledEvent = false;
    while (!xmlReader.atEnd()) {
        if (xmlReader.readNextStartElement())
        {
            if ( xmlReader.name() == "eventList" ) //List start
            {
                //we do nothing, it's just a list
            }
            else if ( xmlReader.name() == "event" ) //New Event
            {
                if ( filledEvent == true ) //We've added data to the last event, so add it
                {
                    addEvent(tempEvent);
                    //qDebug() << tempEvent.eventName << ":" << tempEvent.eventTime.toString("MM/dd/yyyy hh:mm");
                }

                 tempEvent = Event();
                 filledEvent = false;
            }
            else //Event Data
            {
                if ( xmlReader.name() == "eventName" )
                {
                    tempEvent.eventName = xmlReader.readElementText();
                }
                if ( xmlReader.name() == "eventDescription" )
                {
                    tempEvent.eventDescription = xmlReader.readElementText();
                }
                if ( xmlReader.name() == "eventDateTime" )
                {
                    QString readDateTime = xmlReader.readElementText();
                    QDateTime eventDateTime;
                    tempEvent.eventTime = eventDateTime.fromString(readDateTime,"MM-dd-yyyy hh:mm");
                    filledEvent = true;
                }
                if ( xmlReader.name() == "eventTimeZone" )
                {
                    tempEvent.eventTimeZone = xmlReader.readElementText().toInt();
                }
                if ( xmlReader.name() == "thirtyMinuteWarning" )
                {
                    if ( xmlReader.readElementText().toInt() == 1 )
                    {
                        tempEvent.thirtyMinuteWarning = true;
                    }
                    else
                    {
                        tempEvent.thirtyMinuteWarning = false;
                    }
                }
                if ( xmlReader.name() == "fifteenMinuteWarning" )
                {
                    if ( xmlReader.readElementText().toInt() == 1 )
                    {
                        tempEvent.fifteenMinuteWarning = true;
                    }
                    else
                    {
                        tempEvent.fifteenMinuteWarning = false;
                    }
                }
                if ( xmlReader.name() == "fiveMinuteWarning" )
                {
                    if ( xmlReader.readElementText().toInt() == 1 )
                    {
                        tempEvent.fiveMinuteWarning = true;
                    }
                    else
                    {
                        tempEvent.fiveMinuteWarning = false;
                    }
                }
                //qDebug() << xmlReader.readElementText() << ":" << xmlReader.name();
            }
        }
    }

    if ( filledEvent == true ) //We've added data to the last event, so add it
    {
        addEvent(tempEvent);
    }

    //close reader and flush file
    xmlReader.clear();
    xmlFile.close();



}

void hudClock::infoButtonGroupClicked( int eventID )
{
    for ( int i = 0; i < eventList.size(); i++ )
    {
        if ( eventList[i].buttonID == eventID )
        {


            QString message;
            message = "Event:  " + eventList[i].eventName + "\n";
            message = message + "Description: " + eventList[i].eventDescription + "\n\n";
            message = message + "Time: " + eventList[i].eventTime.toString(systemLocale.dateTimeFormat()) + "\n";

                QMessageBox about(QMessageBox::NoIcon, "Event Information", message,
                                          QMessageBox::Ok);
              about.setTextInteractionFlags(Qt::TextSelectableByMouse);

              if ( windowFlags()&Qt::WindowStaysOnTopHint )
              {
                  about.setWindowFlags(about.windowFlags()|Qt::WindowStaysOnTopHint);
              }
              about.exec();

            return;
        }
    }







}

void hudClock::plusButtonGroupClicked( int eventID )
{
    for ( int i = 0; i < eventList.size(); i++ )
    {
        if ( eventList[i].buttonID == eventID )
        {
            if ( eventList[i].timerSecs > 0 )
            {
                eventList[i].eventTime = QDateTime::currentDateTime().addSecs(eventList[i].timerSecs);
                eventList[i].soundAlarm = true;
                eventList[i].fiveMinuteWarning = true;
                eventList[i].fifteenMinuteWarning = true;
                eventList[i].thirtyMinuteWarning = true;
            }
            return;
        }
    }
}



void hudClock::removeButtonGroupClicked(int eventID)
{
    for ( int i = 0; i < eventList.size(); i++ )
    {
        if ( eventList[i].buttonID == eventID )
        {

            QLabel * removed = eventLabelList[i];
            eventLabelList.removeAt(i);
            delete removed;

            QAbstractButton * removedButton = removeButtonGroup.button(eventID);
            removeButtonGroup.removeButton(removedButton);
            delete removedButton;

            if ( eventList[i].timerSecs > 0 ) //Timer
            {
                QAbstractButton * removedPlusButton = plusButtonGroup.button(eventID);
                plusButtonGroup.removeButton(removedPlusButton);
                delete removedPlusButton;
            }
            else //Event
            {
                QAbstractButton * removedInfoButton = infoButtonGroup.button(eventID);
                infoButtonGroup.removeButton(removedInfoButton);
                delete removedInfoButton;
            }

            eventList.removeAt(i);
            adjustSize();
            return;
        }
    }

}

void hudClock::addTimerActionClicked()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("QInputDialog::getText()"),
                                            tr("Timer Name:"), QLineEdit::Normal,
                                            lastTimerText, &ok);
       if (ok && !text.isEmpty())
       {
           lastTimerText = text;
           QString time = QInputDialog::getText(this, tr("QInputDialog::getText()"),
                                                   tr("Timer Amount\n\nFormat: hh:mm:ss,\n\15 min is 15:00\n\2 hours 15.5is 2:15:30"), QLineEdit::Normal,
                                                   lastTimerTime, &ok);
              if (ok && !time.isEmpty())
              {
                  lastTimerTime = time;
                  int secs=0;
                  int pos;
                  QString subsection;

                  pos = time.lastIndexOf(":");
                  if ( pos >= 0 ) //Has a : left, min:sec
                  {

                      subsection = time.right(time.length()-(pos+1));

                      secs += subsection.toInt(); //Add seconds
                      time = time.left(pos);
                      pos = time.lastIndexOf(":");
                      if ( pos  >= 0 ) //Has a : left, hours:min
                      {

                          subsection = time.right(time.length()-(pos+1));

                          secs += subsection.toInt() * 60; //Add Minutes
                          time = time.left(pos);

                          secs += time.toInt() * 60 * 60; //Add Hours

                      }
                      else
                      {

                        secs += 60 * time.toInt();
                      }
                  }
                  else
                  {

                      secs += time.toInt();
                  }
                //Now to add the event

                    addTimer(text, secs);


              }

       }
}


void hudClock::setVolume(float volume)
{
    alarmSound->setVolume(volume);
}
