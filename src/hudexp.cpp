#include "hudexp.h"
#include "ui_hudexp.h"
#include <QDebug>
#include "sotacalcs.h"
#include <QMessageBox>



hudExp::hudExp(QWidget *parent) :
    hudWindow(parent),
    ui(new Ui::hudExp)
{

    ui->setupUi(this);
    ui->tblExpLookup->setModel(&expLookupModel);
    ui->tblExpLookup->verticalHeader()->hide();

    connect(&expLookupModel,SIGNAL(expSignal(int,int)),this, SLOT(updateExp(int,int)));

    //This must be at the end of every HudWindow constructor in order to properly setup transparent backgrounds.
   finishSetup();
}

hudExp::~hudExp()
{
    delete ui;
}

void hudExp::on_btnExpMode_clicked()
{

    //Mode 0 is all kills
    //Mode 1 is current kills
    //Mode 2 is library

    if ( ui->btnExpMode->text() == "Mode (ALL)")
    {
        ui->btnExpMode->setText("Mode (CURRENT)");
        expLookupModel.setMode(1);
    }
    else if ( ui->btnExpMode->text() == "Mode (CURRENT)")
    {
        ui->btnExpMode->setText("Mode (LIBRARY)");
        expLookupModel.setMode(2);
    }
    else if ( ui->btnExpMode->text() == "Mode (LIBRARY)")
    {
        ui->btnExpMode->setText("Mode (ALL)");
        expLookupModel.setMode(0);

    }
}

void hudExp::updateExp(int value, int secspassed)
{
    ui->lblTotalExp->setText(QString::number(value));
    if ( ui->btnExpMode->text() == "Mode (CURRENT)")
    {
        ui->lblTotalExp->setText(
                    QString::number((secspassed/3600) % 60 ) + ":"
                    + QString::number((secspassed/60) % 60 ) + ":"
                    + QString::number(secspassed % 60)+ "    " + ui->lblTotalExp->text()
                    );
    }


    if ( advLevel != -1 ) //We've used stats already
    {
        qlonglong percentThousandsToNextLevel = ((value*10000) / sotaCalcs::getPAExperienceForNextLevel(advLevel));
        double percentToNextLevel = (double)percentThousandsToNextLevel/100;

        ui->lblTotalExp->setText(
                             ui->lblTotalExp->text() + "(" + QString::number(percentToNextLevel,'f',2) + "%)"
                );
    }
}

void hudExp::registerKill(QString name, QString target)
{
    expLookupModel.registerKill(name,target);
}

void hudExp::setAdvLevel(int tempAdvLevel)
{
    advLevel = tempAdvLevel;
}


void hudExp::on_btnExpResetCurrent_clicked()
{

    expLookupModel.clearCurrentKills();
}

void hudExp::on_btnExpSaveLibrary_clicked()
{
    expLookupModel.saveLibrary();
}


void hudExp::help(QPoint /*pos*/)
{


    QString message;
    message = "Experience Window\n";
    message = message + "This window will show you the estimated experience you're gaining, using a lookup list and the chatlogs\n";
    message = message + "The experience file is expList.umu, and is stored in your main HUD directory if you want to share it\n";
    message = message + "This tab has 3 modes, changable by the top-left button\n";
    message = message + "Current: Every kill since the list was last reset via the reset current button\n";
    message = message + "Library: A list of all entries in your library\n";
    message = message + "ALL: A list of every kill in this log file\n";
    message = message + "If you have used /stats, it will also show a % by the current exp amount, showing what % of a level it thinks you've earned with those kills\n";
    message = message + "\n";
    message = message + "Function:\n";
    message = message + "As things die near you, they will show up on the list\n";
    message = message + "If they already have an entry in the library, it will load them from that\n";
    message = message + "However if it is not currently in the library, they will make a new entry and they will have a value of 0 in the ExpPerKill column\n";
    message = message + "In this situation, you will have to set it yourself!\n";
    message = message + "\n";
    message = message + "Setting EXP Values:\n";
    message = message + "To do that, turn off -all- skills (set to maintain), then record your exp value.\n";
    message = message + "Then go kill the creature you're wanting to find out about, and record the new experience value.\n";
    message = message + "Subtract Value #2 from Value #1 and that gives you the experience per kill.\n";
    message = message + "To enter this into the hud, double-click the 0 in the exp per kill column next to the appropriate monster, then type in your number\n";
    message = message + "Be sure to hit SAVE LIBRARY afterwards!\n";

    QMessageBox about(QMessageBox::NoIcon, "EXP Help", message,
                                  QMessageBox::Ok);

    about.setTextInteractionFlags(Qt::TextSelectableByMouse);


    if ( isOnTop() )
    {
    about.setWindowFlags(about.windowFlags()|Qt::WindowStaysOnTopHint);
    }

    about.exec();


}
