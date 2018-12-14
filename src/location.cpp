#include "location.h"
#include <QDebug>
#include <QHeaderView>
#include <QRegExp>


Location::Location()
{
}


Location::Location(QString temp, QTableWidget * outputtemp, Config * tempConfig)
{
    appConfig = tempConfig;
    name = temp;
    tblOutput = outputtemp;
    tblOutput->setSortingEnabled(true);
    tblOutput->setColumnCount(6);
    tblOutput->setHorizontalHeaderItem(0, new QTableWidgetItem("Source"));
    tblOutput->setHorizontalHeaderItem(1, new QTableWidgetItem("Target"));
    tblOutput->setHorizontalHeaderItem(2, new QTableWidgetItem("Skill"));
    tblOutput->setHorizontalHeaderItem(3, new QTableWidgetItem("Damage"));
    tblOutput->setHorizontalHeaderItem(4, new QTableWidgetItem("Count"));
    tblOutput->setHorizontalHeaderItem(5, new QTableWidgetItem("Avg"));
    tblOutput->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);


}
QString Location::getName()
{
    return name;
}




void Location::parseLine(QString temp)
{

    QString source = ""; //Intialized because if for some reason any parsing fails i don't want a null or segfault.
    QString target = ""; //I'm just nice like that.
    QString skill = "";
    QString damage = "";
    bool crit = false;
    int pos; //No we don't care if pos is null.
    //Parse a combat line
    if ( temp.contains("attacks") && ( temp.contains("and hits, dealing") || temp.contains("out of reach") || temp.contains("and is blocked") ))
    {
        if ( temp.contains("and hits, dealing")  )
        {
            //Everything before attacks is the source name
            pos = temp.indexOf("attacks");
            source = temp.left(pos-1);
            temp = temp.mid(pos+8,-1); //right after attacks
            //Everything now before "and hits" is the target
            pos = temp.indexOf("and hits");
            target = temp.left(pos-1);
            temp = temp.mid(pos+18,-1); //This should get us past "and hits, dealing "

            //Now to extract damage
            pos = temp.indexOf("points of");
            damage = temp.left(pos-1);
            temp = temp.mid(pos+10,-1);

            //Is it a critical hit
            if ( temp.contains("critical damage") )
            {

                //Do we care?
                if ( appConfig->ShowCriticalHitsSeperate )
                {
                    //Yes, we care!
                    crit = true;
                }
            }

            //If it has a skill attached.
            if (temp.contains("from "))
            {
                pos = temp.indexOf("from ");
                temp = temp.mid(pos+5,-1);
                skill = temp.left(temp.length()-1); //Leave off the period

            }
            else
            {
                skill = "Normal";
            }


            //We make crits special by adding a + to their skillname
            if ( crit )
            {
                skill = skill + "+";
            }


           updateLocationsStatsTable(source,target,skill,damage.toInt(),true,true,true,true,true,true);
        }
        else if ( temp.contains("and hits, dealing"))
        {
            //Umuri Maxwell attacks Xavara, the Fire Pillar and is out of reach, dealing no damage from Obsidian Arrow.
            QRegExp rx("(.*) attacks (.*) and is out of reach, dealing no damage from (.*).", Qt::CaseInsensitive);

            if ( rx.indexIn(temp,0) < 0)
            {
               //We didn't find shit
            }
            else
            {

                target = rx.cap(1);
                damage = rx.cap(3);
                source = rx.cap(2);
                skill = "Out of Reach";



            }
        }
        else if ( temp.contains("is blocked")) //blocked
        {
            //Skeleton Archer attacks Umuri Maxwell and is blocked, dealing no damage.
            QRegExp rx("(.*) attacks (.*) and is blocked, dealing no damage", Qt::CaseInsensitive);

            if ( rx.indexIn(temp,0) < 0)
            {
               //We didn't find shit
            }
            else
            {

                target = rx.cap(1);
                source = rx.cap(2);
                skill = "Block";

            updateLocationsStatsTable(source,target,skill,0,false,true,false,true,true,true);
            }
       }
    }
    else if ( temp.contains("is healed") )
    {

        //'[6/14/2016 9:27:09 PM] Lich Mage is healed for 3 points of health.'
        //'Water Elemental <Guardian Spif> is healed for 2 points of health by Guardian Spif.'
        //Everything before is healed is the target name
        pos = temp.indexOf("is healed for");
        target = temp.left(pos-1);
        temp = temp.mid(pos+14,-1); //right after attacks

        if ( temp.contains("health by")) //We were healed by someone else
        {
            //Everything now before "and hits" is the target
            pos = temp.indexOf("of health by");
            source = temp.mid(pos+13,-1);
            source = source.left(source.length()-1); //Remove the period.
        }
        else
        {
              source = target;
        }
        //Amount of healing
        if ( temp.contains("points of health") )
        {
            pos = temp.indexOf("points of health");
        }
        else
        {
            pos = temp.indexOf("point of health");
        }
        damage = temp.left(pos-1);


        skill = "Heal";

        updateLocationsStatsTable(source,target,skill,damage.toInt(),true,true,true,true,true,true);
    }
    else if ( temp.contains("tried to attack") )
    {
        if ( temp.contains("and missed")) //Misses
        {
            //'Umuri Maxwell tried to attack Elf Fighter and missed.'
            //Everything before tried to attack is the source name
            pos = temp.indexOf("tried to attack");
            source = temp.left(pos-1);
            temp = temp.mid(pos+16,-1); //right after attacks
            //Everything now before "and hits" is the target
            pos = temp.indexOf("and missed");
            target = temp.left(pos-1);
            skill = "Miss";

            updateLocationsStatsTable(source,target,skill,0,false,true,false,true,true,true);
        }
        else if ( temp.contains("dodges.")) //Dodges
        {
          //'Umuri Maxwell tried to attack Elf Fighter and dodged.'
          //Everything before tried to attack is the source name
          pos = temp.indexOf("tried to attack");
          source = temp.left(pos-1);
          temp = temp.mid(pos+16,-1); //right after attacks
          //Everything now before " but" is the target
          pos = temp.indexOf(" but ");
          target = temp.left(pos);
          skill = "Dodge";

          updateLocationsStatsTable(source,target,skill,0,false,true,false,true,true,true);
        }
    }

    else if ( temp.contains("has been slain by") )  //Killing Blows
    {
          //Everything before has been slain is the target name
          pos = temp.indexOf("has been slain by");
          target = temp.left(pos-1);
          temp = temp.mid(pos+18,-1); //right after by
          //Everything now before "'s" is the source
          pos = temp.indexOf("'s");
          source = temp.left(pos-1);
          temp = temp.mid(pos+3,-1); ////This gets us past


          //Now to extract killing blow weapon
          if ( appConfig->ShowKillingBlowSkillUsed )
          {
            skill = "A Killing Blow - " + temp.left(temp.length()-1);
          }
          else
          {
            skill = "A Killing Blow";
          }
        updateLocationsStatsTable(source,target,skill,0,false,true,false,true,true,true);
    }

    else if ( temp.contains("of damage from Tap Soul") )
    {
        QRegExp rx("(.*) takes (.*) point(?:s)? of damage from Tap Soul", Qt::CaseInsensitive);

        if ( rx.indexIn(temp,0) < 0)
        {
           //We didn't find shit
        }
        else
        {

            target = rx.cap(1);
            damage = rx.cap(2);
            source = "";
            skill = "Tap Soul";



        }
        updateLocationsStatsTable(source,target,skill,damage.toInt(),true,true,true,false,true,true);


    }
    else if ( temp.contains("of damage from Chaotic Feedback.") )
    {
        QRegExp rx("(.*) takes (.*) point(?:s)? of damage from Chaotic Feedback.", Qt::CaseInsensitive);

        if ( rx.indexIn(temp,0) < 0)
        {
           //We didn't find shit
        }
        else
        {

            target = rx.cap(1);
            damage = rx.cap(2);
            source = "";
            skill = "Chaotic Feedback";



        }
        updateLocationsStatsTable(source,target,skill,damage.toInt(),true,true,true,false,true,true);


    }
    else if ( temp.contains("of focus by Tap Soul.") )
    {
        //Umuri Maxwell adds 10 points of focus by Tap Soul.
        QRegExp rx("(.*) adds (.*) point(?:s)? of focus by Tap Soul.", Qt::CaseInsensitive);

        if ( rx.indexIn(temp,0) < 0)
        {
           //We didn't find shit
        }
        else
        {

            target = rx.cap(1);
            damage = rx.cap(2);
            source = "";
            skill = "Tap Soul Focus Regen";



        }
        updateLocationsStatsTable(source,target,skill,damage.toInt(),true,true,true,false,true,true);


    }

    lines.push_back(temp);


}


void Location::updateLocationsStatsTable(QString Source, QString Target, QString Skill, int Damage,  bool UpdateDam,bool UpdateCount, bool UpdateAVG, bool SourceMatch, bool TargetMatch, bool SkillMatch)
{
    // 0 - Source
    // 1 - Target
    // 2 - Skill
    // 3 - Damage
    // 4 - Count
    // 5 - Average
    bool found = false;


    for ( int i = 0; i < tblOutput->rowCount(); i++)
    {
        if (   ( !SourceMatch || tblOutput->item(i,0)->text() == Source )
            && ( !TargetMatch || tblOutput->item(i,1)->text() == Target )
            && ( !SkillMatch || tblOutput->item(i,2)->text() == Skill )
            )     //we have the right source
        {

            if ( UpdateDam )
             {
                tblOutput->item(i,3)->setData(Qt::EditRole, tblOutput->item(i,3)->data(Qt::EditRole).toInt() + Damage );
             }
           if ( UpdateCount )
            {
               tblOutput->item(i,4)->setData(Qt::EditRole, tblOutput->item(i,4)->data(Qt::EditRole).toInt() + 1 );
            }

           if ( UpdateAVG )
            {
               tblOutput->item(i,5)->setData(Qt::EditRole, tblOutput->item(i,3)->data(Qt::EditRole).toFloat() / tblOutput->item(i,4)->data(Qt::EditRole).toFloat() );
            }
            found = true;
            break;
        }
    }
    if (!found)
    {
        QTableWidgetItem *item;

        tblOutput->insertRow(tblOutput->rowCount() );

        tblOutput->setItem(tblOutput->rowCount()-1, 0, new QTableWidgetItem(Source));
        tblOutput->setItem(tblOutput->rowCount()-1, 1, new QTableWidgetItem(Target));
        tblOutput->setItem(tblOutput->rowCount()-1, 2, new QTableWidgetItem(Skill));

        if ( UpdateDam )
        {
            item = new QTableWidgetItem;
            item->setData(Qt::EditRole, Damage);
            tblOutput->setItem(tblOutput->rowCount()-1, 3, item);
        }
        else
        {
            tblOutput->setItem(tblOutput->rowCount()-1, 3, new QTableWidgetItem(""));
        }
        if ( UpdateCount )
        {
            item = new QTableWidgetItem;
            item->setData(Qt::EditRole, 1);
            tblOutput->setItem(tblOutput->rowCount()-1, 4, item);
        }
        else
        {
            tblOutput->setItem(tblOutput->rowCount()-1, 4, new QTableWidgetItem(""));
        }
        if ( UpdateAVG )
        {
            item = new QTableWidgetItem;
            item->setData(Qt::EditRole, (float)Damage);
            tblOutput->setItem(tblOutput->rowCount()-1, 5, item);
        }
        else
        {
        tblOutput->setItem(tblOutput->rowCount()-1, 5, new QTableWidgetItem(""));
        }

    }


}

void Location::setSortingEnabled(bool state)
{
    tblOutput->setSortingEnabled(state);
}
