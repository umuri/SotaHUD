#include "huddps.h"
#include <QPainter>
#include <QFontMetrics>
#include <QMouseEvent>
#include <QRect>
#include <QTextItem>
#include <QDebug>
#include <QMenu>
#include <QInputDialog>






hudDPS::hudDPS(QWidget *parent) :
    hudWindow(parent),brush(QColor(75,25,25))
{
    startTimer(500);

    setAutoFillBackground(true);


    //Settings Buttons
    QAction * setBarHeightAction = new QAction("Set Bar Height");
    settingsActionList.append(setBarHeightAction);
    connect(setBarHeightAction, &QAction::triggered, this, &hudDPS::setBarHeightClicked);


    QAction * setLabelWidthAction = new QAction("Set Name Width");
    settingsActionList.append(setLabelWidthAction);
    connect(setLabelWidthAction, &QAction::triggered, this, &hudDPS::setLabelWidthClicked);

    QAction * setDPSTimeAction = new QAction("Set Time Length");
    settingsActionList.append(setDPSTimeAction);
    connect(setDPSTimeAction, &QAction::triggered, this, &hudDPS::setDPSTimeClicked);

    QAction * setMinDPSAction = new QAction("Set Min DPS");
    settingsActionList.append(setMinDPSAction);
    connect(setMinDPSAction, &QAction::triggered, this, &hudDPS::setMinDPSClicked);

    QAction * setMaxDPSAction = new QAction("Set Max DPS");
    settingsActionList.append(setMaxDPSAction);
    connect(setMaxDPSAction, &QAction::triggered, this, &hudDPS::setMaxDPSClicked);

    //This must be at the end of every HudWindow constructor in order to properly setup transparent backgrounds.
   finishSetup();
}



void hudDPS::paintBargraph(QPainter &painter)
{

    QPen p(QColor(214,171,83));
    p.setWidth(1);
    painter.setPen(p);
    painter.setBrush(brush);



    QFontMetrics fm(painter.font());
    int y_lbl = margin+(barHeight/2);

    if (( values.size() <= 0 ) || ( bars.size() <= 0 ) )
    {
        return;
    }

    for(size_t i=0, s = values.size(); i < s; ++i)
    {
        painter.drawRect(bars[i]);
        QRect k(margin,margin+((margin+barHeight)*i),labelWidth,barHeight);
        painter.drawText(k, Qt::AlignCenter|Qt::TextWordWrap,labels[i]);
        QString lbl = "("+QString::number(values[i])+")";
        painter.drawText(width()/2,y_lbl,lbl);
        y_lbl += margin+barHeight;
    }

    //if(!ylabel.isEmpty())
    {//-fm.width(ylabel)/2

      //  painter.drawText(height()/2,width()/2,"ylabel"+ylabel);
    }
}

void hudDPS::parseLine(QString tempLine, QDateTime timeTemp)
{
    //Nukem attacks Hem-Sareed Ghairmute and hits, dealing 26 points of critical damage from Bow.
    //Fire Elemental <Umuri Maxwell> attacks Elven Mage and hits, dealing 6 points of damage.
    //Elven Mage attacks Fire Elemental <Umuri Maxwell> and hits, dealing no damage.
    //QRegExp rx("(.*) takes (.*) point(?:s)? of damage from Tap Soul", Qt::CaseInsensitive);

    setUpdatesEnabled(false);
    QRegExp rx("(.*) attacks (.*) and hits, dealing (.*) point(?:s)? of (?:critical )?damage.*", Qt::CaseInsensitive);
    QString target, source, damage, weapon;

    //qDebug() << tempLine;
    if ( rx.indexIn(tempLine,0) < 0)
    {
       //We didn't find shit
    }
    else
    {
        //qDebug() << rx.cap(0) << ":" << rx.cap(1) << ":" << rx.cap(2) << ":" << rx.cap(3);
        bool found = false;
        std::vector<dpsEntry> * current;
        for ( unsigned int i = 0; i < sources.size(); i++ )
        {
            if ( sources[i].source == rx.cap(1) )
            {
                found = true;
                current = &sources[i].entries;
            }
        }

        if ( !found )
        {
            dpsSource newSource;
            newSource.source = rx.cap(1);
            sources.push_back(newSource);
            current = &sources[sources.size()-1].entries;
        }



        dpsEntry newEntry;
        newEntry.time = timeTemp;

        newEntry.target = rx.cap(2);
        newEntry.amount = rx.cap(3).toInt();
        //newEntry.weapon = rx.cap(4);

        current->push_back(newEntry);

        //qDebug() << rx.cap(1) << ":" << newEntry.time << ":" << newEntry.target << ":" << newEntry.amount << ":" << newEntry.weapon;
    }


    setUpdatesEnabled(true);
    updated = true;

}

void hudDPS::paintEvent(QPaintEvent * /*event */)
{
    QPainter painter(this);
    paintBargraph(painter);
}

void hudDPS::mouseMoveEvent(QMouseEvent *event)
{
    //if(!show_tooltip)
        return;
    QPoint pos = event->pos();
    auto it = std::find_if(bars.begin(),bars.end(),[pos](const QRect& r){return r.contains(pos);});
    if(it != bars.end())
    {
        //auto i = std::distance(bars.begin(),it);
        //setToolTip(getLabel(i));
    }
    else if(!toolTip().isEmpty())
        setToolTip("");
}

void hudDPS::resizeEvent(QResizeEvent *event)
{
    hudWindow::resizeEvent(event);
    recalcBasicValues();
}

void hudDPS::recalcBasicValues()
{

    if ( sources.size() <= 0 )
    {
        bars.clear();
        return;
    }

    values.clear();
    labels.clear();




    for ( size_t i = 0; i < sources.size();i++)
    {
        labels.push_back(sources[i].source);
        double amount = 0;

        for ( unsigned int j = 0; j < sources[i].entries.size();j++ )
        {
            amount = amount + sources[i].entries[j].amount;
        }
        amount = amount / SECSFORDPS;

        values.push_back(amount);
        //qDebug() << sources[i].source << ":" << amount;

    }
    span = (max_val - min_val) * 1.025;


    double barwidth;

    int w = width() - ((margin * 6) + labelWidth );

    double factor = (((double)w) /span );


    if(bars.size() != values.size())
        bars.resize(values.size());

    int x = labelWidth + margin*2;

    for(int i=0, s = values.size(); i < s; ++i)
    {
        barwidth = 1 + (std::max((double)0,values[i]-min_val) * factor);

        bars[i].setRect(x,margin + (( margin + barHeight) * i ),barwidth,barHeight);
        //y += margin + barHeight;
    }



}

void hudDPS::timerEvent(QTimerEvent * /*e*/)
{

    for ( size_t i = 0; i < sources.size(); i++ )
    {
        for ( size_t j = 0; j < sources[i].entries.size(); j++ )
        {
            if ( sources[i].entries[j].time.secsTo(QDateTime::currentDateTime()) > SECSFORDPS )
            {
                updated = true;
                if (sources[i].entries.size() <= 1 )
                {

                    if ( sources.size() > 1 )
                    {
                        sources.erase(sources.begin() + i);
                        i = i-1;
                        j=0;
                        break;
                    }
                    else
                    {
                        sources.clear();
                        return;
                    }
                }
                else
                {
                    sources[i].entries.erase(sources[i].entries.begin() + j);
                    j=j-1;
                }



            }
        }
    }


    if ( updated == true )
    {
        updated = false;


        recalcBasicValues();
        repaint();
    }
}





QString hudDPS::getWindowSettings()
{
    //Main Window Settings
    // x,y,width,height,opacity,visible
    QString temp = ""
            + QString::number(SECSFORDPS) + ","
            + QString::number(barHeight) + ","
            + QString::number(labelWidth) + ","
            + QString::number(min_val) + ","
            + QString::number(max_val);

    temp = temp + "," + hudWindow::getWindowSettings();
    return temp;

}
void hudDPS::setWindowSettings(QString settings)
{
    QRegExp rx("([^,]*),([^,]*),([^,]*),([^,]*),([^,]*),(.*)", Qt::CaseSensitive);

    //qDebug() << settings;
    if ( rx.indexIn(settings,0) >= 0 )
    {
        SECSFORDPS = rx.cap(1).toInt();
        barHeight = rx.cap(2).toInt();
        labelWidth = rx.cap(3).toInt();
        min_val = rx.cap(4).toInt();
        max_val = rx.cap(5).toInt();

        //qDebug() << rx.cap(4);
        hudWindow::setWindowSettings(rx.cap(6));
    }

}



void hudDPS::setBarHeightClicked()
{

    bool ok;
    QString inputRequest = "Current Height: " + QString::number(barHeight) + "   New Height?";
    int i = QInputDialog::getInt(this, "New Bar Height",
                                 inputRequest, barHeight, 1, 500, 1, &ok);
    if (ok)
        barHeight = i;

    updated = true;
}

void hudDPS::setDPSTimeClicked()
{

    bool ok;
    QString inputRequest = "Current time monitored(secs): " + QString::number(SECSFORDPS) + "   New time(secs)?";
    int i = QInputDialog::getInt(this, "Secs to average DPS",
                                 inputRequest, SECSFORDPS, 1, 3600, 1, &ok);
    if (ok)
        SECSFORDPS = i;

    updated = true;
}


void hudDPS::setLabelWidthClicked()
{

    bool ok;
    QString inputRequest = "Current Width(px): " + QString::number(labelWidth) + "   New width(px)?";
    int i = QInputDialog::getInt(this, "Name Width(px)",
                                 inputRequest, labelWidth, 5, 250, 1, &ok);
    if (ok)
        labelWidth = i;

    updated = true;
}

void hudDPS::setMaxDPSClicked()
{

    bool ok;
    QString inputRequest = "Current Max DPS: " + QString::number(max_val) + "   New Max DPS?";
    int i = QInputDialog::getInt(this, "DPS Scale Max",
                                 inputRequest, max_val, 0, 10000, 1, &ok);
    if (ok)
        max_val = i;

    updated = true;
}

void hudDPS::setMinDPSClicked()
{

    bool ok;
    QString inputRequest = "Current Min DPS: " + QString::number(min_val) + "   New Min DPS?";
    int i = QInputDialog::getInt(this, "DPS Scale Min",
                                 inputRequest, min_val, 0, 10000, 1, &ok);
    if (ok)
        min_val = i;

    updated = true;
}





