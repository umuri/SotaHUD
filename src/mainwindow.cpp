#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QDateTime>
#include <QScrollBar>
#include <QStandardPaths>
#include <QContextMenuEvent>
#include <QProgressDialog>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>
#include <QFileInfo>
#include "notification.h"
#include <QXmlStreamReader>
#include <QCloseEvent>





MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);



    //Define all our chat Regexps in an easy to find place:
    HarvestRegExp.setPattern("You harvested an item \\((.*) x([0-9]*)\\)\\."); //[1/26/2018 6:50:15 PM] You harvested an item (Spider Silk x1).
    LootRegExp.setPattern("You looted an item \\((.*) x([0-9]*)\\)\\."); //[1/28/2018 12:23:03 PM] You looted an item (Corpse Wax x1).


    appConfig.loadConfig();
    AppDataPathPrefix  = appConfig.appDataPath;

    questsController = new questsPlusController(&appConfig);

    //FINDME: ADDWINDOW
    mapWindow = new hudMap();
    expWindow = new hudExp();
    questWindow = new hudQuest(this, questsController); //Add arguments for quests later
    statsWindow = new hudStats();
    clockWindow = new hudClock(this, &appConfig, &popupManager);
    decoWindow = new hudDecoGrid();
    combatWindow = new hudCombat(this, &appConfig);
    dpsWindow = new hudDPS(this);

    //FINDME: ADDWINDOW
    //Default Positions

    questWindow->initialPosition(0.1,0.5);
    mapWindow->initialPosition(0.1,0.2);
    expWindow->initialPosition(0.3,0.1);
    statsWindow->initialPosition(0.85,0.8);
    clockWindow->initialPosition(0.35,0.05);
    dpsWindow->initialPosition(0.7,0.1);
    combatWindow->initialPosition(0.02,0.5);



    salesChecker = new SalesExporter(&appConfig,&popupManager);

    //Sounds get setup first because we adjust volume on them later.
    warning1Sound.setSource(QUrl::fromLocalFile(":/warning1.wav"));
    warning2Sound.setSource(QUrl::fromLocalFile(":/warning2.wav"));
    messageSound.setSource(QUrl::fromLocalFile(":/message.wav"));
    waveSound.setSource(QUrl::fromLocalFile(":/wave.wav"));
    saleSound.setSource(QUrl::fromLocalFile(":/sale.wav"));
    logonSound.setSource(QUrl::fromLocalFile(":/logon.wav"));
    logoffSound.setSource(QUrl::fromLocalFile(":/logoff.wav"));

    connect(QApplication::clipboard(), SIGNAL(dataChanged()),
               this, SLOT(CheckForSaleToPass()));

    loadWindowSettings();

    timerID = startTimer(250);


    controlPointTimer = new QTimer(this);
    connect(controlPointTimer, SIGNAL(timeout()), this, SLOT(controlPointTimerEvent()));
    mainTrackPlayerTimer = new QTimer();
    connect(mainTrackPlayerTimer, SIGNAL(timeout()), this, SLOT(mainTrackPlayerTimerEvent()));




    threadRest = 0;

    setVolumeUniversal(appConfig.Volume);
    characterName = appConfig.CharacterName;


    if ( appConfig.StartOnTop )
    {
        ui->actionStay_On_Top->setChecked(true);
        toggleOnTop(true);
    }
    else
    {
        ui->actionStay_On_Top->setChecked(false);
        toggleOnTop(false);
    }
    if ( appConfig.StartTrackPlayer )
    {
        mapWindow->setTrackedPlayerChecked(true);
    }


    QDateTime pastDayStart = QDateTime::currentDateTime();;
    QDateTime pastDayEnd = QDateTime::currentDateTime();;



    //Set last read so we don't re-parse through it again
    lastRead = QDateTime::currentDateTime();






    ui->tblCraftingSalvage->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    this->setWindowTitle("Umuri's SOTA HUD - " + characterName);
    ui->centralWidget->layout()->setContentsMargins(1, 1, 1, 1);



    QString pathTempPrefix = AppDataPathPrefix;
    pathTempPrefix = pathTempPrefix + "ChatLogs/SotAChatLog_" + characterName;
    currentPlayerDataPath = AppDataPathPrefix + "CurrentPlayerData.xml";
    QDateTime now = QDateTime::currentDateTime();

    QString pathTemp;

    if ( appConfig.ParsePastDay )
    {

        int currentDay = 0;

        QDateTime pastStart = appConfig.pastDayStart;
        QDateTime pastEnd = appConfig.pastDayEnd;





        while ( pastStart < pastEnd )
        {
            currentDay++;
            pathTemp = pathTempPrefix + "_" + pastStart.toString(appConfig.FileDateFormat) + ".txt";
            QFile processDay(pathTemp);
            if ( processDay.exists() )
            {
              processDay.open(QIODevice::ReadOnly);
              QTextStream processStream(&processDay);

              while(!processStream.atEnd())
              {
                  QString line = processStream.readLine();
                  readBuffer.enqueue(line);

              }
               processDay.close();
             }

            pastStart = pastStart.addDays(1);
        }

    }

    pathTemp = pathTempPrefix + "_" + now.toString(appConfig.FileDateFormat) + ".txt";

    myFile = new QFile(pathTemp);
    if ( myFile->exists() )
    {
      myFile->open(QIODevice::ReadOnly);
    }
    else
    {
      setWindowTitle(windowTitle() + " LOG FILE NOT FOUND");
    }


    setWindowTitle(windowTitle() + "                                                " + QString::number(MajorVersion) + "." + QString::number(MinorVersion) + "." + QString::number(RevisionVersion) +"");

     in = new QTextStream(myFile);

     if ( appConfig.ParseCurrentDay )
     {

        while(!in->atEnd())
        {

             QString line = in->readLine();
             readBuffer.enqueue(line);
         }


    }



    connect(ui->actionStay_On_Top, SIGNAL(toggled(bool)), this, SLOT(toggleOnTop(bool)));
    connect(ui->actionFrameless, SIGNAL(toggled(bool)), this, SLOT(toggleFrameless(bool)));


    connect(ui->sldOpacity, SIGNAL(valueChanged(int)), this, SLOT(setOpacityValue(int)));
    connect(ui->actionCurrent_Visible_Table_csv, &QAction::triggered, this, &MainWindow::exportCurrentVisibleAsCSV);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::showAboutDialog);
    connect(ui->actionCredits, &QAction::triggered, this, &MainWindow::showCreditsDialog);
    connect(ui->actionConfiguration, &QAction::triggered, this, &MainWindow::showConfigDialog);








    this->installEventFilter(this);


    mainTrackPlayerTimer->start(1000);



    //Window Controls Initialization - FINDME: ADDWINDOW
    connect(ui->sldQuestOpacity, SIGNAL(valueChanged(int)), this, SLOT(setQuestOpacityValue(int)));
    connect(ui->sldMapOpacity, SIGNAL(valueChanged(int)), this, SLOT(setMapOpacityValue(int)));
    connect(ui->sldExpOpacity, SIGNAL(valueChanged(int)), this, SLOT(setExpOpacityValue(int)));
    connect(ui->sldStatsOpacity, SIGNAL(valueChanged(int)), this, SLOT(setStatsOpacityValue(int)));
    connect(ui->sldClockOpacity, SIGNAL(valueChanged(int)), this, SLOT(setClockOpacityValue(int)));
    connect(ui->sldCombatOpacity, SIGNAL(valueChanged(int)), this, SLOT(setCombatOpacityValue(int)));
    connect(ui->sldDPSOpacity, SIGNAL(valueChanged(int)), this, SLOT(setDPSOpacityValue(int)));

}



MainWindow::~MainWindow()
{
    questsController->saveQuests();
    myFile->close();
    killTimer(timerID);
    delete ui;
}

void MainWindow::parseLine(QString temp)
{

    threadRest++;
    if ( threadRest % maxThreadRest == 0 )
    {
        QApplication::processEvents();
        threadRest = 0;
    }
    if ( temp.length() < 5 ) //Throw out any stub lines
    {
        return;
    }
    QDateTime qdttemp;
    QString datetemp;
    QString trimmedLine;

    //Debug Section
    if (( temp[0] == '[') && (temp.indexOf("]") < 40 )) //Contains the date
    {



         //Pull out the date string
         datetemp = temp.mid(1, temp.indexOf("]")-1);

         qdttemp = qdttemp.fromString(datetemp,appConfig.LogDateTimeFormat);
         if ( !qdttemp.isValid() )
         {
            //Ugly defaults
            qdttemp = qdttemp.fromString(datetemp,"M/d/yyyy h:mm:ss ap");
             if ( !qdttemp.isValid() )
             {
                qdttemp = qdttemp.fromString(datetemp,"M/d/yyyy hh:mm:ss");
                if ( !qdttemp.isValid() )
                {
                   qdttemp = qdttemp.fromString(datetemp,"MM/dd/yyyy hh:mm:ss");
                   if ( !qdttemp.isValid() )
                   {
                      qdttemp = qdttemp.fromString(datetemp,"M/d/yyyy hh:mm:ss");
                   }
                }
             }
         }




         //ui->txtDebug->append(qdttemp.toString("yyyy-MM-dd hh:mm:ss"));


         //Now we split out the line without the date, since we have that
         trimmedLine = temp.mid(temp.indexOf("]")+2);

         //Now we have two objects, qdttemp which holds the datetime for processing, and trimmedLine which holds the string.

                                                 if ( DEBUG )
                                                 {
                                                 qDebug() << "Date extracted: " << qdttemp.toString("yyyy-MM-dd hh:mm:ss") << "||" << trimmedLine;

                                                 }



        //This is basic combat stuff
        //Example lines:
        //[6/19/2016 10:39:02 AM] Umuri Maxwell tried to attack Large Grey Wolf and missed.
        //[6/19/2016 10:39:03 AM] Large Grey Wolf attacks Umuri Maxwell and hits, dealing 3 points of damage.
        if ( trimmedLine.indexOf("Entering") == 0 )
        {
            int pos = 0;
            QString locationNameTemp;
            trimmedLine = trimmedLine.mid(9,-1); //Remove "Entering "
            pos = trimmedLine.indexOf("from");
            locationNameTemp = trimmedLine.left(pos-1);

            combatWindow->enterLocation(locationNameTemp);


        }
        else if (( trimmedLine.contains("attacks") && ( trimmedLine.contains("and hits, dealing") || trimmedLine.contains("is out of reach") ))
                 || ( trimmedLine.contains("tried to attack") && ( trimmedLine.contains("and missed") || trimmedLine.contains("dodges.") || trimmedLine.contains("blocked")) )
                 || ( trimmedLine.contains("is healed") && trimmedLine.contains("of health") )
                 || ( trimmedLine.contains("has been slain by") )
                 || ( trimmedLine.contains("of damage from Chaotic Feedback") )
                 || ( trimmedLine.contains("points of focus by Tap Soul.") )
                 || ( trimmedLine.contains("of damage from Tap Soul.")  )
                 ) //Basic combat
        {
            combatWindow->parseLine(trimmedLine);

            if ( ( trimmedLine.contains("and hits, dealing") )
                 && ( qdttemp.secsTo(QDateTime::currentDateTime()) < 1800 )
                 )
            {
                dpsWindow->parseLine(trimmedLine, qdttemp);
            }
            if ( trimmedLine.contains("has been slain by") )
            {
                int pos = trimmedLine.indexOf("has been slain by ");
                if ( pos >= 1 ) // Valid match;
                {
                    QString target = trimmedLine.left(pos-1);
                    //qDebug() << currentLocation->getName() << ":" << target;
                    expWindow->registerKill(combatWindow->getCurrentLocationName(),target);

                    if ( !bulkLoading )
                    {
                        questsController->registerKill(target);
                    }

                }
            }

        }
        else if ( ( trimmedLine.contains("Successfully salvaged an item"))
                 )
        {
            parseSalvage(temp);
        }
        else if (  trimmedLine.contains("Good news! The item")
                   && trimmedLine.contains("can be collected at the bank") )
        {
            //Good news! The item (Gold Crown of the Obsidians  x5) you placed on sale has been purchased by Banshe fireborn on Sunday, August 28, 2016 5:20 PM. Proceeds from the sale (6,500 gold) can be collected at the bank in Owl's Head.
            //[8/26/2016 1:23:32 AM] Good news! The item (Gold Crown of the Obsidians  x10) you wanted to buy has been purchased from Licemeat on Friday, August 26, 2016 1:23 AM (Purchase price: 10,000 gold). Items from this sale can be collected at the bank in Brittany Alleys.
            ui->listVendors->addItem(trimmedLine);

            if ( ( !bulkLoading ) )
            {
                if ( appConfig.SalesSound )
                {
                    saleSound.play();
                }
                if ( appConfig.SalesPopup )
                {
                    popupManager.addNotification("You made a sale!",trimmedLine,appConfig.SalesTime);
                }
            }

        }
        else if ( ( trimmedLine.contains("Item crafted: "))
                  || ( trimmedLine.contains("destroyed during crafting.") )
                 )
        {
            parseCrafting(temp);
        }
        else if (
                 (  trimmedLine.contains("Item") && ( trimmedLine.contains("added to inventory")) )
                 || ( trimmedLine.contains("damage to secondary durability") )
                 || ( trimmedLine.contains("damage to primary durability") )
                 )
        {
            //Do nothing for now

        }
        else if ( HarvestRegExp.indexIn(trimmedLine) != -1 )
        {
            //[1/26/2018 6:50:15 PM] You harvested an item (Spider Silk x1).
            //qDebug() << "[MW] Harvest: " << HarvestRegExp.cap(1) << ":" << HarvestRegExp.cap(2);
            if ( !bulkLoading )
            {
                questsController->registerLoot(HarvestRegExp.cap(1),HarvestRegExp.cap(2).toInt());
            }
        }
        else if ( LootRegExp.indexIn(trimmedLine) != -1 )
        {
            //[1/28/2018 12:23:03 PM] You looted an item (Corpse Wax x1).
            //qDebug() << "[MW] Loot: " << LootRegExp.cap(1) << ":" << LootRegExp.cap(2);

            if ( !bulkLoading )
            {
                questsController->registerLoot(LootRegExp.cap(1),LootRegExp.cap(2).toInt());
            }

        }
        else if ( ( trimmedLine.contains("You caught nothing")) || ( trimmedLine.contains("You gained an item") && trimmedLine.contains("from fishing")))
        {
            parseFishing(temp);
        }
        else if ( trimmedLine.contains("is now offline") )
        {
            if ( !bulkLoading )
            {
                bool notify = false;
                if ( appConfig.loginNotificationListOn )
                {
                    for ( int i = 0; i < appConfig.loginNotificationList.count(); ++i )
                    {
                        if ( trimmedLine.contains(appConfig.loginNotificationList[i]) )
                        {
                            notify = true;
                        }
                    }
                }
                else
                {
                    notify = true;
                }
                if ( notify )
                {
                    if ( appConfig.LogonLogoffSound )
                    {
                        logonSound.play();
                    }
                    if ( appConfig.LogonLogoffPopup )
                    {
                        popupManager.addNotification("Logged Off",trimmedLine,appConfig.LogonLogoffTime);
                    }
                }
            }
            ui->listLoginsLogoffs->addItem(temp);

        }
        else if ( trimmedLine.contains ("is now online") )
        {
            if ( !bulkLoading )
            {
                if ( appConfig.LogonLogoffSound )
                {
                    logonSound.play();
                }
                if ( appConfig.LogonLogoffPopup )
                {
                    popupManager.addNotification("Logged On",trimmedLine,appConfig.LogonLogoffTime);
                }
            }
            //Code for online/offline player tracking is here.
            ui->listLoginsLogoffs->addItem(temp);


        }

        else if ( trimmedLine.contains("skill") && trimmedLine.contains ("has increased to level"))
        {
            //Code for skillgain Tracking
            ui->listSkillGain->addItem( temp);
        }
        //This is the /stats command, parse it out
        else if ( trimmedLine.contains("AdventurerLevel: ") && trimmedLine.contains ("GameTime: ") )
         {
            statsWindow->parseStatsCommand(trimmedLine, qdttemp);

            expWindow->setAdvLevel(statsWindow->getAdvLevel());
         }
         else if ( trimmedLine.contains(":") ) //This is probably chat of some kind
            {
                QString temp = "(To " + appConfig.CharacterName + ")";

                QRegExp rx("([^\\(]*)(?: \\(To (.*)\\))?: (.*)$", Qt::CaseSensitive);

                //This mess of a regexp tries to match all chat of the form X(To Y?):Z
                //Where X[rx.cap(1)] is the speaker, Y[rx.cap(2)] is the channel/whisper target if it exists, and Z[rx.cap(3)] is the chat message

                if ( rx.indexIn(trimmedLine,0) >= 0 )
                {
                  //This is standard form chat
                  //qDebug() << "[MT]Chat: " << rx.capturedTexts();
                  if (rx.cap(2) == appConfig.CharacterName)
                  {
                  //This is a whisper



                        if ( ( !bulkLoading ) )
                        {

                            questsController->registerChatLog(rx.capturedTexts(),ChatType::Whisper);

                            if ( appConfig.WhisperSound )
                            {
                                messageSound.play();
                            }
                            if ( appConfig.WhisperPopup )
                            {
                                popupManager.addNotification("Whisper",trimmedLine,appConfig.WhisperTime);
                            }
                        }
                    }

                }
                if ( !bulkLoading)
                {
                    questsController->registerChatLog(rx.capturedTexts(),ChatType::Chat);
                }

                    addTextToTxt(ui->listChat ,trimmedLine );
            }
         else if (trimmedLine.contains("takes no damage") ) //We took no damage from something!
            {
                //Nope, not doing anything right now;
            }
        else if (trimmedLine.contains("utters the phrase") ) //Spellcast Text
           {
            QRegExp rx("(.*) utters the phrase (.*)\\.", Qt::CaseSensitive);
            if ( rx.indexIn(trimmedLine,0) >= 0 )
            {
                if ( !bulkLoading)
                {  questsController->registerChatLog(rx.capturedTexts(),ChatType::Spell_Cast);  }
            }

               //Nope, not doing anything right now;
           }
         else
         {
            QString tempDebug;
            tempDebug = "Line with no options but has a date: " + qdttemp.toString("yyyy-MM-dd hh:mm:ss") + "||" +  trimmedLine;
            addTextToTxt(ui->listDebug,tempDebug);

         }
         addTextToTxt(ui->listLog,temp);


    }
    else if ( temp.contains("====== NEW SESSION =====") )
    {
            addTextToTxt(ui->listLog,"==== NEW SESSION ====");
    }
    else     //We don't know what to do with this line, so output it to the debug window
    {
            //qDebug() << "Line with no options:" << temp;
            addTextToTxt(ui->listDebug,temp);
    }

}

void MainWindow::timerEvent(QTimerEvent * /*e*/)
{

    bool updates = false;

    if ( lastRead.date().dayOfWeek() != QDateTime::currentDateTime().date().dayOfWeek() ) //We've moved to the next day
    {

        QString pathTempPrefix;

        pathTempPrefix = AppDataPathPrefix;
        pathTempPrefix = pathTempPrefix + "ChatLogs/SotAChatLog_";
        pathTempPrefix = pathTempPrefix + characterName;


        QString pathTemp;


        pathTemp = pathTempPrefix + "_" + QDateTime::currentDateTime().toString() + ".txt";

        QFileInfo check_file(pathTemp);
        if (check_file.exists() && check_file.isFile())
        {

               myFile->close();

               myFile = new QFile(pathTemp);
               if ( myFile->exists() ) //Redundant, i know, but what the hay!
               {
                 myFile->open(QIODevice::ReadOnly);
               }

               in = new QTextStream(myFile);
           }

    }



    //Once per second, we'll read in any new data that came into the file
    if ( appConfig.ParseCurrentDay )
    {
        while(!in->atEnd())
        {
                                                    if ( DEBUG )
                                                    {
                                                    qDebug() << "Begin line read";
                                                    }

            QString line = in->readLine();

                                                    if ( DEBUG )
                                                    {
                                                    qDebug() << "End line read";
                                                    }
            if ( line.length() > 4 ) //Throw out any stub lines
            {
                readBuffer.push_back(line);
            }
            lastRead = QDateTime::currentDateTime();

        }
    }
    if ( readBuffer.isEmpty() )
    {
        return;
    }
    disableUpdateEvents();
    int count = 0;
    int total = 0;
    if ( readBuffer.length() > 100 )
    {
        popupManager.disableNotifications();
        bulkLoading = true;

       total = readBuffer.length();
       progress = new QProgressDialog("Parsing entries...", "Does Nothing", 0, total, this);
        progress->setWindowModality(Qt::WindowModal);

        progress->show();
       progress->setCancelButton(0);
       progress->setValue(0);
        while ( !readBuffer.isEmpty() )
        {
            count++;
            parseLine(readBuffer.dequeue());
            updates = true;
            if ( count % 50 == 0 )
            {
               progress->setValue(count);
            }
        }
        progress->setValue(total);


        popupManager.enableNotifications();
        bulkLoading = false;
    }
    else while (( !readBuffer.isEmpty() ) && ( count < 500 ) )
        {
            count++;
            parseLine(readBuffer.dequeue());
            updates = true;
        }

    if (( updates )  && ( ui->actionScroll_to_bottom_on_chat_log_debug->isChecked() ) )//If we've updated
    {
        if ( ui->listLog->isVisible() )
        {
            ui->listLog->scrollToBottom();
        }
        else if ( ui->listChat->isVisible() )
        {
            ui->listChat->scrollToBottom();
        }
        if ( ui->listDebug->isVisible() )
        {
            ui->listDebug->scrollToBottom();
        }
    }
    enableUpdateEvents();
}



void MainWindow::insertIntoTable(QTableWidget * table, QString tempKey, QString tempValue)
{
    table->insertRow(table->rowCount() );
    table->setItem(table->rowCount()-1, 0, new QTableWidgetItem(tempKey));
    table->setItem(table->rowCount()-1, 1, new QTableWidgetItem(tempValue));

}


void MainWindow::toggleOnTop(bool state)
{

    Qt::WindowFlags flags = windowFlags();

    flags &= ~Qt::WindowFullscreenButtonHint; //This is here to allow the constructor to call properly
    if (state)
    {    
       flags &= ~Qt::WindowStaysOnBottomHint;
       flags |= Qt::WindowStaysOnTopHint;
    }
    else
    {
        flags &= ~Qt::WindowStaysOnTopHint;
        flags |= Qt::WindowStaysOnBottomHint;        
    }
    setWindowFlags(flags);

    //FINDME: ADDWINDOW
    questWindow->toggleOnTop(state);
    mapWindow->toggleOnTop(state);
    expWindow->toggleOnTop(state);
    statsWindow->toggleOnTop(state);
    clockWindow->toggleOnTop(state);
    combatWindow->toggleOnTop(state);
    dpsWindow->toggleOnTop(state);
    show();
}

void MainWindow::toggleFrameless(bool state)
{

    Qt::WindowFlags flags = windowFlags();

    if (state) //Enable
    {

       flags |= Qt::FramelessWindowHint;
       ui->menuBar->hide();
       statusBarStyleTemp = ui->statusBar->styleSheet();
       ui->statusBar->setStyleSheet("QStatusBar::tab { height: 1px; width: 1px; }");
       ui->statusBar->hide();
       if ( ui->actionHide_Menu_bar_when_frameless->isChecked())
       {
           mainTabStyleTemp = ui->tabMainTab->tabBar()->styleSheet();
           //ui->tabMainTab->tabBar()->setStyleSheet("QTabBar::tab { height: 1px; width: 1px; }");
           ui->tabMainTab->tabBar()->hide();
       }
       ui->sldOpacity->hide();
       ui->lblOpacity->hide();

    }
    else //Disable
    {

        flags ^= Qt::FramelessWindowHint;
        ui->menuBar->show();
        ui->statusBar->show();
        if ( ui->actionHide_Menu_bar_when_frameless->isChecked())
        {
        ui->tabMainTab->tabBar()->show();
        //ui->tabMainTab->tabBar()->setStyleSheet(mainTabStyleTemp);
        }
        ui->statusBar->setStyleSheet(statusBarStyleTemp);
        ui->statusBar->show();
        ui->sldOpacity->show();
        ui->lblOpacity->show();

    }
    setWindowFlags(flags);

    //FINDME: ADDWINDOW
    questWindow->toggleFrameless(state);
    mapWindow->toggleFrameless(state);
    expWindow->toggleFrameless(state);
    statsWindow->toggleFrameless(state);
    clockWindow->toggleFrameless(state);
    combatWindow->toggleFrameless(state);
    dpsWindow->toggleFrameless(state);
    show();
}

void MainWindow::toggleFramelessCheck()
{
    ui->actionFrameless->toggle();
}

void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{

   if ( windowFlags()&Qt::FramelessWindowHint )
   {
       framelessAct = new QAction(tr("Turn off &Frameless"), this);
        framelessAct->setStatusTip(tr("Turn off Frameless"));
   }
   else
   {

       framelessAct = new QAction(tr("Turn on &Frameless"), this);
           framelessAct->setStatusTip(tr("Turn on Frameless"));
   }

   connect(framelessAct, &QAction::triggered, this, &MainWindow::toggleFramelessCheck);

    QMenu menu(this);
    menu.addAction(framelessAct);
    menu.addSeparator();
    QMenu * windowMenu = menu.addMenu("View Tab");
    for ( int i = 0; i < ui->tabMainTab->count(); i++ )
    {
        QAction * tabAction = new QAction();
        tabAction->setData(i);
        tabAction->setText( ui->tabMainTab->tabText(i) );

        windowMenu->addAction(tabAction);
    }

    connect(windowMenu, SIGNAL(triggered(QAction*)), this, SLOT(changeTabViaContext(QAction*)));
    menu.exec(event->globalPos());
}

void MainWindow::changeTabViaContext(QAction * tabIndexAction)
{
    ui->tabMainTab->setCurrentIndex(tabIndexAction->data().toInt());
}

void MainWindow::setOpacityValue(int newValue)
{
    double newOpacity;
    newOpacity = newValue / (double)100;
    setWindowOpacity(newOpacity);
}


void MainWindow::mousePressEvent(QMouseEvent *event) {
    m_nMouseClick_X_Coordinate = event->x();
    m_nMouseClick_Y_Coordinate = event->y();
}

void MainWindow::mouseMoveEvent(QMouseEvent *event) {
    if ( ( windowFlags() &  Qt::FramelessWindowHint ) ==  Qt::FramelessWindowHint )
    move(event->globalX()-m_nMouseClick_X_Coordinate,event->globalY()-m_nMouseClick_Y_Coordinate);
}



void MainWindow::parseSalvage(QString temp)
{
    //Example:  Successfully salvaged an item (Long Bow ) for Wood Scrap  x23.
    QString Item;
    QString Salvage;
    QString Amount;
    int pos;

    pos = temp.indexOf('(');
    temp = temp.mid(pos+1,-1);
    pos = temp.indexOf(')');
    Item = temp.left(pos-1);
    pos = temp.indexOf(" for ");
    temp = temp.mid(pos + 5);
    pos = temp.lastIndexOf('x');
    Salvage = temp.left(pos-1);
    temp=temp.mid(pos+1);
    Amount = temp.left(temp.length()-1);



    bool found = false;
    for ( int i = 0; i < ui->tblCraftingSalvage->rowCount(); i++)
    {
        if (
                ui->tblCraftingSalvage->item(i,0)->text() == Item
                && ui->tblCraftingSalvage->item(i,1)->text() == Salvage
            )//we have the right source
        {
            ui->tblCraftingSalvage->item(i,2)->setText(QString::number(ui->tblCraftingSalvage->item(i,2)->text().toInt() + Amount.toInt()));
            ui->tblCraftingSalvage->item(i,3)->setText(QString::number(ui->tblCraftingSalvage->item(i,3)->text().toInt() + 1));
            ui->tblCraftingSalvage->item(i,4)->setText(QString::number(ui->tblCraftingSalvage->item(i,2)->text().toFloat() /  ui->tblCraftingSalvage->item(i,3)->text().toFloat()));
            found = true;
            break;

        }




    }
    if (!found)
    {
        ui->tblCraftingSalvage->insertRow(ui->tblCraftingSalvage->rowCount() );
        ui->tblCraftingSalvage->setItem(ui->tblCraftingSalvage->rowCount()-1, 0, new QTableWidgetItem(Item));
        ui->tblCraftingSalvage->setItem(ui->tblCraftingSalvage->rowCount()-1, 1, new QTableWidgetItem(Salvage));
        ui->tblCraftingSalvage->setItem(ui->tblCraftingSalvage->rowCount()-1, 2, new QTableWidgetItem(Amount));
        ui->tblCraftingSalvage->setItem(ui->tblCraftingSalvage->rowCount()-1, 3, new QTableWidgetItem("1"));
        ui->tblCraftingSalvage->setItem(ui->tblCraftingSalvage->rowCount()-1, 4, new QTableWidgetItem(Amount));

    }
}

void MainWindow::parseCrafting(QString temp)
{
    ui->listCraftingCrafting->addItem(temp);
}

void MainWindow::parseFishing(QString temp)
{
    ui->listCraftingFishing->addItem(temp);
}



void MainWindow::addTextToTxt(QListWidget * windowToAdd, QString text)
{

    if ( windowToAdd->count() > appConfig.MaxDisplay)
    {


        for ( int i = appConfig.MaxClearLength; i>=0; i-- )
        {
             delete windowToAdd->takeItem(i);
        }


    }

    windowToAdd->addItem(text);


}

void MainWindow::disableUpdateEvents()
{
    ui->listChat->hide();
    ui->listDebug->hide();
    ui->listLog->hide();
    combatWindow->disableUpdates();



}
void MainWindow::enableUpdateEvents()
{
    ui->listChat->show();
    ui->listDebug->show();
    ui->listLog->show();
    combatWindow->enableUpdates();
}


void MainWindow::exportCurrentVisibleAsCSV()
{
    int currentTabIndex;


    currentTabIndex = ui->tabMainTab->currentIndex();

    QWidget * current = ui->tabMainTab->widget(currentTabIndex);
    //qDebug() << "   ";
    QList<QTabBar *> allTabBars = current->findChildren<QTabBar *>();

    if ( allTabBars.count() == 1 ) //If it's a tab bar, we go one step further
    {
        QList<QTabWidget *> allTabWidgets = current->findChildren<QTabWidget *>();
        if ( allTabWidgets.count() == 1 ) //We have a tabbar in this
        {
            current = allTabWidgets[0]->widget(allTabWidgets[0]->currentIndex());
        }
    }

    while (strcmp(current->metaObject()->className(),"QWidget") == 0 )
    {
        //qDebug() << "Widget";
        if  (strcmp(current->metaObject()->className(),"QWidget") == 0 )
        {
           QList<QWidget *> allChildren = current->findChildren<QWidget *>();
           current = allChildren[0];
            for ( int i = 0; i < allChildren.count(); i++ )
            {


              if ( strcmp(allChildren[i]->metaObject()->className(),"QTableWidget" ) == 0 )
                {
                    current = allChildren[i];
                    break;
                }
                else if ( strcmp(allChildren[i]->metaObject()->className(),"QListWidget" ) == 0 )
                {
                    current = allChildren[i];
                    break;
                }

                if ( i + 1 == allChildren.count())
                {
                    current = NULL;
                    return;
                }
            }
        }




    }

    if ((strcmp(current->metaObject()->className(),"QTableWidget") == 0 ) ||(strcmp(current->metaObject()->className(),"QListWidget") == 0 ) )
    {
        QString fileName = QFileDialog::getSaveFileName(this, "Save as csv", appConfig.DefaultSavePath, "CSV (*.csv)");
        QFile data(fileName);
        if(data.open(QFile::WriteOnly |QFile::Truncate | QIODevice::Text))
        {
            QTextStream output(&data);


            if (strcmp(current->metaObject()->className(),"QTableWidget") == 0 )
            {
                QTableWidget * table = (QTableWidget*)current;

                for ( int i = 0; i < table->rowCount(); i++ )
                {
                    for ( int j = 0; j < table->columnCount(); j++)
                    {

                        output << table->item(i,j)->text().replace(",","\\,");
                        if ( j + 1 != table->columnCount() )
                        {
                          output << ',';
                        }
                    }

                    output << '\n';
                }
            }
            else if (strcmp(current->metaObject()->className(),"QListWidget") == 0 )
            {

                QListWidget * list = (QListWidget *)current;


                for ( int i = 0; i < list->count(); i ++)
                {
                      output << list->item(i)->text() << '\n';
                }


            }
        }
        data.close();
    }






}

void MainWindow::showConfigDialog()
{
    appConfig.changeSettings();
    characterName = appConfig.CharacterName;


    setVolumeUniversal(appConfig.Volume);
}

void MainWindow::showCreditsDialog()
{
    QString message;

    message = "Sound Credits:\n";
    message = message + "All sounds can be found at freesound.org and are licensed under CreativeCommons Attribution 3.0";
    message = message + "Control point Warning Sound: Distant Horns (218488) by DANMITCH3LL\n";
    message = message + "http://www.freesound.org/people/DANMITCH3LL/sounds/218488/ \n";
    message = message + "Control point Wave Sound: Roman Horn - Announcing (342364) by HistoriaLover\n";
    message = message + "http://www.freesound.org/people/HistoriaLover/sounds/342364/\n";
    message = message + "Sales Sound: Cha Ching (351304) by Guest\n";
    message = message + "http://www.freesound.org/people/guest/sounds/351304/\n";
    message = message + "Message Sound: Chicken Single Alarm Call (316920) by Rudmer_Rotteveel\n";
    message = message + "https://www.freesound.org/people/Rudmer_Rotteveel/sounds/316920/\n";
    message = message + "Clock Alarm Sound: Angelus Bell (78381) by Peaveycroquette\n";
    message = message + "https://www.freesound.org/people/Peaveycroquette/sounds/78381/\n";
    message = message + "Logoff Sound: Door close 2 (53269) by THE_bizniss\n";
    message = message + "https://www.freesound.org/people/THE_bizniss/sounds/53269/\n";
    message = message + "Logon Sound: DoorOpen4ss (36827) by csproductions\n";
    message = message + "https://www.freesound.org/people/csproductions/sounds/36827/\n";





    QMessageBox about(QMessageBox::NoIcon, "About", message,
                                  QMessageBox::Ok);
      about.setTextInteractionFlags(Qt::TextSelectableByMouse);

      if ( windowFlags()&Qt::WindowStaysOnTopHint )
      {
          about.setWindowFlags(about.windowFlags()|Qt::WindowStaysOnTopHint);
      }
      about.exec();


}

void MainWindow::showAboutDialog()
{
    QString message;
    message = "Umuri's SOTA Horrendously Unbeautiful Display\n";
    message = message + "Version " + QString::number(MajorVersion) + "." + QString::number(MinorVersion) + "." + QString::number(RevisionVersion) + "\n";
    message = message + "\n";
    message = message + "Contact the developer:\n";
    message = message + "In-game: Umuri Maxwell\n";
    message = message + "Forums: Umuri\n";
    message = message + "Bitcoin: 1GCfMwWoeWVahCymheuztxoRuEoHT8zKgW\n";
    message = message + "Patreon: https://www.patreon.com/Umuri\n";





      QMessageBox about(QMessageBox::NoIcon, "About", message,
                                    QMessageBox::Ok);
        about.setTextInteractionFlags(Qt::TextSelectableByMouse);

        if ( windowFlags()&Qt::WindowStaysOnTopHint )
        {
            about.setWindowFlags(about.windowFlags()|Qt::WindowStaysOnTopHint);
        }
        about.exec();


}

void MainWindow::on_btnControlPoint_clicked()
{

    if ( ui->btnControlPoint->text() == "Start Control Point Timer" )
    {
        controlPointWave = 1;
        controlPointTime = 1;
        ui->btnControlPoint->setText("Reset Control Point Timer");
        controlPointTimer->start(1000);
    }
    else if ( ui->btnControlPoint->text() == "Reset Control Point Timer" )
    {
        controlPointWave = 0;
        ui->btnControlPoint->setText("Start Control Point Timer");
        controlPointTimer->stop();
        ui->lblControlPointStatus->setText("Control Point Timer Off");
    }
}


void MainWindow::controlPointTimerEvent()
{

    controlPointTime += 1;


    QString temp;
    //Clean this string construction up later, split out for debug purposes
    temp = "Wave: " + QString::number(controlPointWave);
    temp = temp + "  Time in wave: " + QString::number( controlPointTime % 120 );
    temp = temp + "  Total Time: " + QString::number( (controlPointTime/60) );
    temp = temp +  ":"+ QString("%1").arg(controlPointTime%60, 2, 10, QChar('0'));


    ui->lblControlPointStatus->setText(temp);
    if (( controlPointTime % 180 ) == 150 && ui->chkControlPoint30Second->isChecked()) //Warning!
    {
        warning1Sound.play();
    }
    if (( controlPointTime % 180 ) == 165 && ui->chkControlPoint15Second->isChecked()) //Warning!
    {
        warning2Sound.play();
    }
    if (( controlPointTime % 180 ) == 0  && ui->chkControlPointNoisy)//Next wave!
    {
        waveSound.play();
        controlPointWave += 1;
    }



}

void MainWindow::mainTrackPlayerTimerEvent()
{

    QFile xmlFile(currentPlayerDataPath);

    if (!xmlFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
                        return;
    }
    QXmlStreamReader xmlReader(&xmlFile);

    double x = 0;
    double y = 0;
    double z = 0;
    QString mapName;
    QString unityName;
    int orientation = -90;

    //Parse the XML until we reach end of it
    while(!xmlReader.atEnd() && !xmlReader.hasError()) {
            // Read next element
            QXmlStreamReader::TokenType token = xmlReader.readNext();
            //If token is just StartDocument - go to next
            if(token == QXmlStreamReader::StartDocument) {
                    continue;
            }
            //If token is StartElement - read it
            if(xmlReader.isStartElement())
            {
                if ( xmlReader.name() == "map_friendly" )
                    {
                       mapName = xmlReader.readElementText();
                    }
                if ( xmlReader.name() == "map_file" )
                    {
                        unityName = xmlReader.readElementText();
                    }
                if ( xmlReader.name() == "loc_x" )
                    {
                        x = xmlReader.readElementText().toDouble();
                    }
                if ( xmlReader.name() == "loc_y" )
                    {
                        y = xmlReader.readElementText().toDouble();
                    }
                if ( xmlReader.name() == "loc_z" )
                    {
                        z = xmlReader.readElementText().toDouble();
                    }
                if ( xmlReader.name() == "orientation" )
                    {
                        orientation = xmlReader.readElementText().toInt();
                    }

            }
    }



    //close reader and flush file
    xmlReader.clear();
    xmlFile.close();

    questWindow->dataChanged();
    //qDebug() << unityName << ":" << x << "," << y << "," << z;
    mapWindow->hudMapTrackPlayerTimerEvent(mapName, unityName, orientation, x,y,z);
    questsController->registerCurrentLocation(unityName,x,y,z);
}

void MainWindow::CheckForSaleToPass()
{
    QString clipboardText = QApplication::clipboard()->text();
    salesChecker->CheckForSale(clipboardText);
}


void MainWindow::setVolumeUniversal(int volume)
{
    float vol;

    vol = (((float)volume) / 100);

    warning1Sound.setVolume(vol);
    warning2Sound.setVolume(vol);
    waveSound.setVolume(vol);
    saleSound.setVolume(vol);
    messageSound.setVolume(vol);
    logonSound.setVolume(vol);
    logoffSound.setVolume(vol);

    clockWindow->setVolume(vol);
}







bool MainWindow::eventFilter( QObject */*o*/, QEvent *e )
{
    if ( e->type() == QEvent::KeyPress ) {
        // special processing for key press
        //QKeyEvent *k = (QKeyEvent *)e;
        //qDebug( "Ate key press %d", k->key() );
        return true; // eat event
    }
    else {
        // standard event processing
        return false;
    }
}



void MainWindow::closeEvent (QCloseEvent */*event*/)
{
    //FINDME: ADDWINDOW
    saveWindowSettings();
    appConfig.saveSettings();
    expWindow->close();
    mapWindow->close();
    statsWindow->close();
    clockWindow->close();
    combatWindow->close();
    dpsWindow->close();
    questWindow->close();
    popupManager.closeAll();



}





void MainWindow::on_btnMapWindow_clicked()
{
    if ( ui->btnMapWindow->isChecked() )
    {
        mapWindow->show();
        ui->btnMapWindow->setChecked(true);
    }
    else
    {
        mapWindow->hide();
        ui->btnMapWindow->setChecked(false);
    }

}


void MainWindow::on_btnDeco_clicked()
{

    if ( ui->btnDeco->isChecked() )
    {
        decoWindow->show();
        ui->btnDeco->setChecked(true);
    }
    else
    {
        decoWindow->hide();
        ui->btnDeco->setChecked(false);
    }

}


    //FINDME: ADDWINDOW - Add an opacity function

void MainWindow::setDPSOpacityValue(int newValue)
{
    double newOpacity;
    newOpacity = newValue / (double)100;
    dpsWindow->setOpacity(newOpacity);
}

void MainWindow::setMapOpacityValue(int newValue)
{
    double newOpacity;
    newOpacity = newValue / (double)100;
    mapWindow->setOpacity(newOpacity);
}


void MainWindow::setExpOpacityValue(int newValue)
{
    double newOpacity;
    newOpacity = newValue / (double)100;
    expWindow->setOpacity(newOpacity);
}

void MainWindow::setStatsOpacityValue(int newValue)
{
    double newOpacity;
    newOpacity = newValue / (double)100;
    statsWindow->setOpacity(newOpacity);
}

void MainWindow::setClockOpacityValue(int newValue)
{
    double newOpacity;
    newOpacity = newValue / (double)100;
    clockWindow->setOpacity(newOpacity);
}
void MainWindow::setCombatOpacityValue(int newValue)
{
    double newOpacity;
    newOpacity = newValue / (double)100;
    combatWindow->setOpacity(newOpacity);
}
void MainWindow::setQuestOpacityValue(int newValue)
{
    double newOpacity;
    newOpacity = newValue / (double)100;
    questWindow->setOpacity(newOpacity);
}



//FINDME: ADDWINDOW - Add button function

void MainWindow::on_btnExpWindow_clicked()
{
    if ( ui->btnExpWindow->isChecked() )
    {
        expWindow->show();
        ui->btnExpWindow->setChecked(true);
    }
    else
    {
        expWindow->hide();
        ui->btnExpWindow->setChecked(false);
    }

}
void MainWindow::on_btnStatsWindow_clicked()
{
    if ( ui->btnStatsWindow->isChecked() )
    {
        statsWindow->show();
        ui->btnStatsWindow->setChecked(true);
    }
    else
    {
        statsWindow->hide();
        ui->btnStatsWindow->setChecked(false);
    }

}

void MainWindow::on_btnClockWindow_clicked()
{
    if ( ui->btnClockWindow->isChecked() )
    {
        clockWindow->show();
        ui->btnClockWindow->setChecked(true);
    }
    else
    {
        clockWindow->hide();
        ui->btnClockWindow->setChecked(false);
    }

}


void MainWindow::on_btnCombatWindow_clicked()
{
    if ( ui->btnCombatWindow->isChecked() )
    {
        combatWindow->show();
        ui->btnCombatWindow->setChecked(true);
    }
    else
    {
        combatWindow->hide();
        ui->btnCombatWindow->setChecked(false);
    }

}

void MainWindow::on_btnDPSWindow_clicked()
{
    if ( ui->btnDPSWindow->isChecked() )
    {
        dpsWindow->show();
        ui->btnDPSWindow->setChecked(true);
    }
    else
    {
        dpsWindow->hide();
        ui->btnDPSWindow->setChecked(false);
    }

}

void MainWindow::on_btnQuestWindow_clicked()
{

    if ( ui->btnQuestWindow->isChecked() )
    {
        questWindow->show();
        ui->btnQuestWindow->setChecked(true);
    }
    else
    {
        questWindow->hide();
        ui->btnQuestWindow->setChecked(false);
    }


}



//FINDME: ADDWINDOW - Add button function


void MainWindow::on_chkMapClickthrough_toggled(bool /*checked*/)
{
    mapWindow->toggleClickThrough();
}


void MainWindow::on_chkExpClickthrough_toggled(bool /*checked*/)
{
    expWindow->toggleClickThrough();
}

void MainWindow::on_chkStatsClickthrough_toggled(bool /*checked*/)
{
    statsWindow->toggleClickThrough();
}
void MainWindow::on_chkClockClickthrough_toggled(bool /*checked*/)
{
    clockWindow->toggleClickThrough();
}
void MainWindow::on_chkCombatClickthrough_toggled(bool /*checked*/)
{
    combatWindow->toggleClickThrough();
}

void MainWindow::on_chkDPSClickthrough_toggled(bool /*checked*/)
{
    dpsWindow->toggleClickThrough();
}

void MainWindow::on_chkQuestClickthrough_toggled(bool /*checked*/)
{
    questWindow->toggleClickThrough();
}




void MainWindow::saveWindowSettings()
{
    //FINDME: ADDWINDOW
    appConfig.mainWindowSettings = getWindowSettings();
    appConfig.mapWindowSettings = mapWindow->getWindowSettings();
    appConfig.expWindowSettings = expWindow->getWindowSettings();
    appConfig.statsWindowSettings = statsWindow->getWindowSettings();
    appConfig.clockWindowSettings = clockWindow->getWindowSettings();
    appConfig.combatWindowSettings = combatWindow->getWindowSettings();
    appConfig.dpsWindowSettings = dpsWindow->getWindowSettings();
    appConfig.questWindowSettings = questWindow->getWindowSettings();

}

void MainWindow::loadWindowSettings()
{
    //FINDME: ADDWINDOW
    //qDebug() << appConfig.mainWindowSettings << endl << appConfig.mapWindowSettings << endl << appConfig.expWindowSettings << endl;
    setWindowSettings(appConfig.mainWindowSettings);
    mapWindow->setWindowSettings(appConfig.mapWindowSettings);
    expWindow->setWindowSettings(appConfig.expWindowSettings);
    statsWindow->setWindowSettings(appConfig.statsWindowSettings);
    clockWindow->setWindowSettings(appConfig.clockWindowSettings);
    combatWindow->setWindowSettings(appConfig.combatWindowSettings);
    dpsWindow->setWindowSettings(appConfig.dpsWindowSettings);
    questWindow->setWindowSettings(appConfig.questWindowSettings);
    ui->sldOpacity->setSliderPosition(windowOpacity()*100);
    ui->sldMapOpacity->setSliderPosition(mapWindow->windowOpacity()*100);
    ui->sldExpOpacity->setSliderPosition(expWindow->windowOpacity()*100);
    ui->sldStatsOpacity->setSliderPosition(statsWindow->windowOpacity()*100);
    ui->sldClockOpacity->setSliderPosition(clockWindow->windowOpacity()*100);
    ui->sldCombatOpacity->setSliderPosition(combatWindow->windowOpacity()*100);
    ui->sldDPSOpacity->setSliderPosition(dpsWindow->windowOpacity()*100);
    ui->sldQuestOpacity->setSliderPosition(questWindow->windowOpacity()*100);
    ui->btnMapWindow->setChecked(mapWindow->isVisible());
    ui->btnExpWindow->setChecked(expWindow->isVisible());
    ui->btnStatsWindow->setChecked(statsWindow->isVisible());
    ui->btnClockWindow->setChecked(clockWindow->isVisible());
    ui->btnCombatWindow->setChecked(combatWindow->isVisible());
    ui->btnDPSWindow->setChecked(dpsWindow->isVisible());
    ui->btnQuestWindow->setChecked(questWindow->isVisible());
    if (mapWindow->windowFlags()&Qt::WindowTransparentForInput )
    {
        ui->chkMapClickthrough->setChecked(true);
        mapWindow->toggleClickThrough(true);
    }
    if (expWindow->windowFlags()&Qt::WindowTransparentForInput )
    {
        ui->chkExpClickthrough->setChecked(true);
        expWindow->toggleClickThrough(true);
    }
    if (statsWindow->windowFlags()&Qt::WindowTransparentForInput )
    {
        ui->chkStatsClickthrough->setChecked(true);
        statsWindow->toggleClickThrough(true);
    }
    if (clockWindow->windowFlags()&Qt::WindowTransparentForInput )
    {
        ui->chkClockClickthrough->setChecked(true);
        clockWindow->toggleClickThrough(true);
    }
    if (combatWindow->windowFlags()&Qt::WindowTransparentForInput )
    {
        ui->chkCombatClickthrough->setChecked(true);
        combatWindow->toggleClickThrough(true);
    }
    if (dpsWindow->windowFlags()&Qt::WindowTransparentForInput )
    {
        ui->chkDPSClickthrough->setChecked(true);
        dpsWindow->toggleClickThrough(true);
    }
    if (questWindow->windowFlags()&Qt::WindowTransparentForInput )
    {
        ui->chkQuestClickthrough->setChecked(true);
        questWindow->toggleClickThrough(true);
    }

}

QString MainWindow::getWindowSettings()
{
    //Main Window Settings
    // x,y,width,height,opacity,visible
    QString temp = ""
            + QString::number(pos().x()) + ","
            + QString::number(pos().y()) + ","
            + QString::number(width()) + ","
            + QString::number(height()) + ","
            + QString::number(windowOpacity()) + ","
            + ((isVisible()) ? "1" : "0" );

    return temp;

}
void MainWindow::setWindowSettings(QString settings)
{
    QRegExp rx("(.*)\\,(.*)\\,(.*)\\,(.*)\\,(.*)\\,(.*)", Qt::CaseSensitive);

    if ( rx.indexIn(settings,0) >= 0 )
    {
        setGeometry(rx.cap(1).toInt(),rx.cap(2).toInt(),rx.cap(3).toInt(),rx.cap(4).toInt());
        setWindowOpacity(rx.cap(5).toFloat());

        //rx.cap(6) visible isn't used yet
    }

}


void MainWindow::changeEvent(QEvent *e)
{

    if(e->type() == QEvent::WindowStateChange) {

        if(isMinimized()) {

// MINIMIZED
            //FINDME: ADDWINDOW
            mapWindow->minimize();
            expWindow->minimize();
            statsWindow->minimize();
            clockWindow->minimize();
            decoWindow->minimize();
            combatWindow->minimize();
            dpsWindow->minimize();
            questWindow->minimize();


        } else {

// NORMAL/MAXIMIZED ETC
            mapWindow->restore();
            expWindow->restore();
            statsWindow->restore();
            clockWindow->restore();
            decoWindow->restore();
            combatWindow->restore();
            dpsWindow->restore();
            questWindow->restore();


        }
    }

    e->accept();

}


