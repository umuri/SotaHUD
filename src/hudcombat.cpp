#include "hudcombat.h"
#include "ui_hudcombat.h"

hudCombat::hudCombat(QWidget *parent, Config * tempConfig) :
    hudWindow(parent),
    ui(new Ui::hudCombat)
{
    ui->setupUi(this);

    appConfig = tempConfig;
    ui->tabCombat->setStyleSheet("QTabWidget::tab-bar {right: 60px; }");

    newTabCount = 0;
    QTableWidget * tblLocation = new QTableWidget();
    ui->tabCombat->addTab(tblLocation, "Default");
    currentLocation = new Location("Homebase", tblLocation, appConfig );
    Locations.append(currentLocation);
    defaultLocation = currentLocation;
    // Create button what must be placed in tabs row
    tb = new QToolButton();
    tb->setText("+");
    ui->tabCombat->setCornerWidget(tb,Qt::TopLeftCorner);
    connect(tb, SIGNAL(clicked()), this, SLOT(newLocationButton()));
    //This must be at the end of every HudWindow constructor in order to properly setup transparent backgrounds.
   finishSetup();
}

hudCombat::~hudCombat()
{
    delete ui;
}

QString hudCombat::getCurrentLocationName()
{
    return currentLocation->getName();
}

void hudCombat::enterLocation(QString locationNameTemp)
{
    int foundLocation = -1;
    bool found = false;
    //Now we see if we already have a tab for this one:
    for ( int i = 0; i < Locations.size(); i ++)
    {
        if ( Locations[i]->getName() == locationNameTemp )
        {
            found = true;
            foundLocation = i;
            break;
        }
    }
    if ( found )
    {
        currentLocation = Locations[foundLocation];
    }
    else
    {
    QTableWidget * tblLocation = new QTableWidget();
    ui->tabCombat->insertTab(1,tblLocation, locationNameTemp);
    currentLocation = new Location(locationNameTemp, tblLocation, appConfig );
    Locations.append(currentLocation);
    ui->tabCombat->setCurrentIndex(1);


    }
}

void hudCombat::parseLine( QString line)
{
    //qDebug() << line;
    if ( currentLocation != defaultLocation )
    {
        defaultLocation->parseLine(line);
    }
    currentLocation->parseLine(line);
}

void hudCombat::newLocationButton()
{
    newTabCount++;
    QTableWidget * tblLocation = new QTableWidget();
    ui->tabCombat->insertTab(1,tblLocation, "New " + QString::number(newTabCount));
    currentLocation = new Location("New "  + QString::number(newTabCount), tblLocation, appConfig );
    Locations.append(currentLocation);
    ui->tabCombat->setCurrentIndex(1);
}

void hudCombat::disableUpdates()
{
    currentLocation->setSortingEnabled(false);
    defaultLocation->setSortingEnabled(false);
}

void hudCombat::enableUpdates()
{
    currentLocation->setSortingEnabled(true);
    defaultLocation->setSortingEnabled(true);
}
