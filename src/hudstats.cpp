#include "hudstats.h"
#include "ui_hudstats.h"
#include "sotacalcs.h"

hudStats::hudStats(QWidget *parent) :
    hudWindow(parent),
    ui(new Ui::hudStats)
{
    ui->setupUi(this);
    ui->tabStatsTab->setStyleSheet("QTabWidget::tab-bar {right: 60px;}");



 /*   ui->tblCombatStats->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tblCraftingStats->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tblMagicStats->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tblMiscStats->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tblStatusStats->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tblTamingStats->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);*/
    //This must be at the end of every HudWindow constructor in order to properly setup transparent backgrounds.
   finishSetup();
}

hudStats::~hudStats()
{
    delete ui;
}




void hudStats::parseStatsCommand(QString temp, QDateTime /*Unused*/)
{


    ui->tblCombatStats->clearContents();
    ui->tblCombatStats->setRowCount(0);
    ui->tblMagicStats->clearContents();
    ui->tblMagicStats->setRowCount(0);
    ui->tblTamingStats->clearContents();
    ui->tblTamingStats->setRowCount(0);
    ui->tblCraftingStats->clearContents();
    ui->tblCraftingStats->setRowCount(0);
    ui->tblMiscStats->clearContents();
    ui->tblMiscStats->setRowCount(0);
    ui->tblStatusStats->clearContents();
    ui->tblStatusStats->setRowCount(0);
    ui->tblCraftingStats->setSortingEnabled(false);
    currentStats.clear();


    QString tempItem;
    QString tempKey;
    QString tempValue;


   //ui->listStatsCommand->clear();
   //ui->listStatsCommand->addItem("Stats updated: " + qdttemp.toString("yyyy-MM-dd hh:mm:ss"));

   QRegExp rx("([A-Za-z]*): ([0-9.-]*)", Qt::CaseInsensitive);

   while ( rx.indexIn(temp,0) >= 0)
   {
        sortStatsCommand(rx.cap(1), rx.cap(2));
        temp = temp.mid(temp.indexOf(rx.cap(2)) + rx.cap(2).length()+1,-1);
   }


   //ui->tblCombat->sortByColumn(0, Qt::AscendingOrder);
   ui->tblCraftingStats->setSortingEnabled(true);
   ui->tblCraftingStats->sortByColumn(0, Qt::AscendingOrder);
   //ui->tblMisc->sortByColumn(0,Qt::AscendingOrder);
   //ui->tblStatus->sortByColumn(0, Qt::AscendingOrder);
   processOverview();

}

int hudStats::getAdvLevel()
{
    return currentStats["AdventurerLevel"].toInt();
}

int hudStats::getProdLevel()
{
    return currentStats["ProducerLevel"].toInt();
}

QString hudStats::getStatistic(QString stats)
{
    return currentStats[stats];
}


void hudStats::sortStatsCommand(QString tempKey, QString tempValue)
{
    currentStats[tempKey]=tempValue;
    if ( //Crafting
            tempKey.contains("Mining") //Gathering skills
         || tempKey.contains("Forestry")
         || tempKey.contains("FieldDressing")
         || tempKey.contains("Foraging")
         || tempKey.contains("Fishing")
         || tempKey.contains("Milling")
         || tempKey.contains("Smelting")
         || tempKey.contains("Textiles")
         || tempKey.contains("Agriculture")
         || tempKey.contains("Tanning")
         || tempKey.contains("Butchery")
         || tempKey.contains("Blacksmithing")
         || tempKey.contains("Carpentry")
         || tempKey.contains("Tailoring")
         || tempKey.contains("Alchemy")
         || tempKey.contains("Cooking")
         || tempKey.contains("Masterwork")
         )
    {
        insertIntoTable(ui->tblCraftingStats, tempKey, tempValue);
    }
    else if ( //Magic
                 tempKey.contains("Chaos")
              || tempKey.contains("Water")
              || tempKey.contains("Fire")
              || tempKey.contains("Air")
              || tempKey.contains("Earth")
              || tempKey.contains("Sun")
              || tempKey.contains("Moon")
              || tempKey.contains("LLife")
              || tempKey.contains("Death")
              || tempKey.contains("Fizzle")
              || tempKey.contains("LLunar")
              || tempKey.contains("Solar")
              || tempKey.contains("Cold")
              || tempKey.contains("HealOther")
              || tempKey.contains("CastTimeMultiplier")

              )
    {
        insertIntoTable(ui->tblMagicStats, tempKey, tempValue);
    }
    else if ( //Taming
                 tempKey.contains("Tame")
              || tempKey.contains("Taming")
              )
    {
        insertIntoTable(ui->tblTamingStats, tempKey, tempValue);
    }
    else if ( //Combat
                 tempKey.contains("Melee")
              || tempKey.contains("Ranged")
              || tempKey.contains("Crit")
              || tempKey.contains("Weapon")
              || tempKey.contains("Shield")
              || tempKey.contains("Armor")
              || tempKey.contains("Attack")
              || tempKey.contains("Combat")
              || tempKey.contains("Polearm")
              || tempKey.contains("Bludgeon")
              || tempKey.contains("AmmoScavengerChance")


              )
    {
        insertIntoTable(ui->tblCombatStats, tempKey, tempValue);
    }
    else if ( //Stats
                 tempKey.contains("LLevel")
              || tempKey.contains("Dexterity")
              || tempKey.contains("Strength")
              || tempKey.contains("Intelligence")
              || tempKey.contains("Carry")
              || tempKey.contains("Health")
              || tempKey.contains("Focus")

              || tempKey.contains("Hidden")
              || tempKey.contains("GameTime")
              || tempKey.contains("Nightvision")
              || tempKey.contains("Hidden")
              || tempKey.contains("Move")
              || tempKey.contains("DamageResistance")
              || tempKey.contains("DamageAvoidance")
              || tempKey.contains("DamageAbsorptionBonus")
              || tempKey.contains("Safe")
              || tempKey.contains("EncumbranceCapacity")
              )//Stats!
    {
        insertIntoTable(ui->tblStatusStats, tempKey, tempValue);
    }
    else
    {
        insertIntoTable(ui->tblMiscStats, tempKey, tempValue);
    }

}

void hudStats::insertIntoTable(QTableWidget * table, QString tempKey, QString tempValue)
{
    table->insertRow(table->rowCount() );
    table->setItem(table->rowCount()-1, 0, new QTableWidgetItem(tempKey));
    table->setItem(table->rowCount()-1, 1, new QTableWidgetItem(tempValue));
}



void hudStats::processOverview()
{
    QLocale locale = QLocale::system();

    //This one is ugly and in html
    QString overview = "";
    int adventurerLevel = currentStats["AdventurerLevel"].toInt();
    int producerLevel = currentStats["ProducerLevel"].toInt();
    overview += "Adventurer Level: " + currentStats["AdventurerLevel"] + "<br>";
    overview += "Producer Level: " + currentStats["ProducerLevel"] + "<br>";


    overview += "<br>";
    overview += "ESTIMATES(NOT ALWAYS RIGHT): <br>";
    overview += "Adv Exp: " + locale.toString(sotaCalcs::getPAExperienceForLevel(adventurerLevel)) + "<br>";
    overview += "Adv Exp to level: " + locale.toString(sotaCalcs::getPAExperienceForNextLevel(adventurerLevel)) + "<br>";
    overview += "Decay per hour: " + locale.toString(sotaCalcs::getPADecay(adventurerLevel,1))  + "<br>";
    overview += "Decay per death(12h): " + locale.toString(sotaCalcs::getPADecay(adventurerLevel,12))  + "<br>";
    overview += "Decay per death(24h): " + locale.toString(sotaCalcs::getPADecay(adventurerLevel,24))  + "<br>";
    overview += "<br>";
    overview += "Prod Exp: " + locale.toString(sotaCalcs::getPAExperienceForLevel(producerLevel)) + "<br>";
    overview += "Prod Exp to level: " + locale.toString(sotaCalcs::getPAExperienceForNextLevel(producerLevel)) + "<br>";


    ui->txtOverview->setHtml(overview);
}

