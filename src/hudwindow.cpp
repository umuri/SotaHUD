#include "hudwindow.h"
#include <QContextMenuEvent>
#include <QMenu>
#include <QDebug>
#include <QDir>
#include <QDesktopWidget>

hudWindow::hudWindow(QWidget * /*temp*/)
{

    toggleFrameless(true);



    setMinimumWidth(90);
    setMinimumHeight(55);

    QString imagePath =  QDir::currentPath() +"/icons/";


    closeButton = new QPushButton(QString(""), this);
    closeButton->setStyleSheet("border:0px;");
    QPixmap closePixmap(imagePath + "close.png");
    QIcon closeButtonIcon(closePixmap);
    closeButton->setFixedSize(13,15);
    closeButton->setIcon(closeButtonIcon);
    closeButton->setIconSize(closePixmap.rect().size());

    if ( !showCloseButton )
    {     closeButton->hide();     }

    pinButton = new QPushButton(QString(""), this);
    pinButton->setStyleSheet("border:0px;");
    pinnedPixMap = QPixmap(imagePath + "pin.png");
    unpinnedPixMap = QPixmap(imagePath + "unpin.png");
    QIcon pinButtonIcon(pinnedPixMap);
    pinButton->setFixedSize(13,15);
    pinButton->setIcon(pinButtonIcon);
    pinButton->setIconSize(pinnedPixMap.rect().size());

    if ( !showPinButton )
    {     pinButton->hide();     }


    settingsButton = new QPushButton(QString(""), this);
    settingsButton->setStyleSheet("border:0px;");
    QPixmap settingsPixmap(imagePath + "settings.png");
    QIcon settingsButtonIcon(settingsPixmap);
    settingsButton->setFixedSize(13,15);
    settingsButton->setIcon(settingsButtonIcon);
    settingsButton->setIconSize(settingsPixmap.rect().size());
    if ( !showSettingsButton )
    {     settingsButton->hide();     }

    helpButton = new QPushButton(QString(""), this);
    helpButton->setStyleSheet("border:0px;");
    QPixmap helpPixmap(imagePath + "qmark.png");
    QIcon helpButtonIcon(helpPixmap);
    helpButton->setFixedSize(13,15);
    helpButton->setIcon(helpButtonIcon);
    helpButton->setIconSize(helpPixmap.rect().size());
    if ( !showHelpButton )
    {     helpButton->hide();     }




    connect(closeButton, &QPushButton::clicked, this, &hudWindow::closeButtonClicked);
    connect(settingsButton, &QPushButton::clicked, this, &hudWindow::settingsButtonClicked);
    connect(helpButton, &QPushButton::clicked, this, &hudWindow::helpButtonClicked);
    connect(pinButton, &QPushButton::clicked, this, &hudWindow::pinButtonClicked);

    //THE FOLLOWING IS SNAFU.
    //You must set the translucent background as true before the first show()
    //or else it won't have done the setup required to toggle it later.
    setWindowFlags(windowFlags()|Qt::Tool);




}

void hudWindow::finishSetup()
{
    setAttribute(Qt::WA_NoSystemBackground,true);
    setAttribute(Qt::WA_TranslucentBackground,true);
    show();
    //This is to reset the background to whatever people want.
    setAttribute(Qt::WA_NoSystemBackground,false);
    setAttribute(Qt::WA_TranslucentBackground,false);
    hide();
    show();
    hide();
}

void hudWindow::initialPosition(float x, float y)
{
    QDesktopWidget widget;
    QRect mainScreenSize = widget.availableGeometry(widget.primaryScreen()); // or screenGeometry(), depending on your need
    int desktopX = mainScreenSize.width();
    int desktopY = mainScreenSize.height();

    show();
    move(desktopX*x,desktopY*y);
    hide();
}

void hudWindow::resizeEvent( QResizeEvent* ) {

    int widthPlacement = width() - 5;
    const int BUTTONSPACING = 13;

    if ( showCloseButton )
    {
        widthPlacement -= qMax(closeButton->width(),BUTTONSPACING);
        closeButton->move( widthPlacement, 5 );
        closeButton->raise();
    }
    else
    {
        closeButton->hide();
    }
    if ( showPinButton )
    {
        widthPlacement -= qMax(pinButton->width(),BUTTONSPACING);
        pinButton->move( widthPlacement, 5 );
        pinButton->raise();
    }
    else
    {
        pinButton->hide();
    }
    if ( showSettingsButton )
    {
        widthPlacement -= qMax(settingsButton->width(),BUTTONSPACING);
        settingsButton->move( widthPlacement, 5 );
        settingsButton->raise();
    }
    else
    {
        settingsButton->hide();
    }

    if ( showHelpButton )
    {
        widthPlacement -= qMax(helpButton->width(),BUTTONSPACING);
        helpButton->move( widthPlacement, 5 );
        helpButton->raise();
    }
    else
    {
        helpButton->hide();
    }

}

void hudWindow::closeButtonClicked()
{
    hide();
    close();

}

void hudWindow::settingsButtonClicked()
{
    settings(QCursor::pos());
}

void hudWindow::helpButtonClicked()
{
    help(QCursor::pos());
}

void hudWindow::minimize()
{
    if ( isVisible() )
    {
        minimized = true;
        hide();
    }

}
void hudWindow::restore()
{
    if ( minimized)
    {
        minimized=false;
        show();
    }

}

void hudWindow::help(QPoint /*pos*/)
{

}
void hudWindow::settings(QPoint Pos)
{

    /*
     *
     *How to add:


     put into constructor of window that inherits hudWindow:
     QAction * newAction = new QAction(tr("Description"), this);
     settingsActionList.append(newAction);

     connect(newAction, &QAction::triggered, this, &class::functionSlot);

     *
     */

    if ( testAttribute(Qt::WA_NoSystemBackground)  || testAttribute(Qt::WA_TranslucentBackground) )
    {
        toggleBackgroundAct = new QAction(tr("Turn on &Background"), this);
         toggleBackgroundAct->setStatusTip(tr("Turn on Background"));
    }
    else
    {

        toggleBackgroundAct = new QAction(tr("Turn off &Background"), this);
            toggleBackgroundAct->setStatusTip(tr("Turn off Background"));
    }

    connect(toggleBackgroundAct, &QAction::triggered, this, &hudWindow::toggleBackgroundCheck);

     QMenu menu(this);
     menu.addAction(toggleBackgroundAct);
     menu.addSeparator();
     if ( !settingsActionList.isEmpty())
     {
         for ( int i = 0; i < settingsActionList.count(); i++)
         {
             menu.addAction(settingsActionList[i]);
         }
     }

     menu.exec(Pos);
}

bool hudWindow::isFrameless()
{
 return windowFlags()&Qt::FramelessWindowHint;
}
bool hudWindow::isClickthrough()
{
    return windowFlags()&Qt::WindowTransparentForInput;
}
bool hudWindow::isOnTop()
{
    return windowFlags()&Qt::WindowStaysOnTopHint;
}

void hudWindow::pinButtonClicked()
{
    bool state = toggleFrameless();
    if ( state ) //We went frameless
    {
        QIcon pinButtonIcon(pinnedPixMap);
        pinButton->setFixedSize(13,13);
        pinButton->setIcon(pinButtonIcon);
        pinButton->setIconSize(pinnedPixMap.rect().size());

    }
    else
    {
        QIcon pinButtonIcon(unpinnedPixMap);
        pinButton->setFixedSize(13,13);
        pinButton->setIcon(pinButtonIcon);
        pinButton->setIconSize(unpinnedPixMap.rect().size());
    }
}

bool hudWindow::toggleOnTop()
{
    if ( windowFlags()&Qt::WindowStaysOnTopHint )
    {
        return toggleOnTop(false);
    }
    else
    {
        return toggleOnTop(true);
    }
}

bool hudWindow::toggleOnTop(bool state)
{

    bool showAfter = this->isVisible();
    bool returnBool = false;

    Qt::WindowFlags flags = windowFlags();

    flags &= ~Qt::WindowFullscreenButtonHint; //This is here to allow the constructor to call properly

    if (state) //Enable
    {
       flags &= ~Qt::WindowStaysOnBottomHint;
       flags |= Qt::WindowStaysOnTopHint;
       returnBool = true;
    }
    else //Disable
    {
        flags &= ~Qt::WindowStaysOnTopHint;
        flags |= Qt::WindowStaysOnBottomHint;
    }

    setWindowFlags(flags);

    if ( showAfter )
    {
        show();
    }

    return returnBool;
}

bool hudWindow::toggleClickThrough()
{
    if ( windowFlags()&Qt::WindowTransparentForInput )
    {
        return toggleClickThrough(false);
    }
    else
    {
        return toggleClickThrough(true);
    }
}


bool hudWindow::toggleClickThrough(bool state)
{
    bool showAfter = this->isVisible();
    bool returnBool = false;
    Qt::WindowFlags flags = windowFlags();

    if ( state ) //Enable
    {
        flags |= Qt::WindowTransparentForInput;
        returnBool = true;

        pinButton->hide();
        settingsButton->hide();
        closeButton->hide();
        helpButton->hide();


    }
    else //Disable
    {
          flags ^= Qt::WindowTransparentForInput;
          if ( showPinButton )
          {
              pinButton->show();
          }
          if ( showCloseButton )
          {
              closeButton->show();
          }
          if ( showSettingsButton )
          {
              settingsButton->show();
          }
          if ( showHelpButton )
          {
              helpButton->show();
          }
    }

    setWindowFlags(flags);

    if ( showAfter )
    {
        show();
    }

    return returnBool;

}




void hudWindow::setOpacity(float value)
{
    setWindowOpacity(value);
}


bool hudWindow::toggleFrameless()
{
    if ( windowFlags()&Qt::FramelessWindowHint )
    {
        return toggleFrameless(false);
    }
    else
    {
        return toggleFrameless(true);
    }

}

bool hudWindow::toggleFrameless(bool state)
{
    bool showAfter = this->isVisible();

    Qt::WindowFlags flags = windowFlags();

    if (state) //Enable
    {

       flags |= Qt::FramelessWindowHint;

    }
    else //Disable
    {

        flags ^= Qt::FramelessWindowHint;
   }
    setWindowFlags(flags);

    if ( showAfter )
    {
        show();
    }

    return state;

}


void hudWindow::closeWindow()
{
    hide();
    close();
}



void hudWindow::contextMenuEvent(QContextMenuEvent *event)
{

    /*
     *
     *How to add:


     put into constructor of window that inherits hudWindow:
     QAction * newAction = new QAction(tr("Description"), this);
     rightClickActionList.append(newAction);

     connect(newAction, &QAction::triggered, this, &class::functionSlot);

     *
     */

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

   connect(framelessAct, &QAction::triggered, this, &hudWindow::toggleFramelessCheck);

    QMenu menu(this);
    menu.addAction(framelessAct);
    menu.addSeparator();
    if ( !rightClickActionList.isEmpty())
    {
        for ( int i = 0; i < rightClickActionList.count(); i++)
        {
            menu.addAction(rightClickActionList[i]);
        }
    }

    menu.exec(event->globalPos());
}


void hudWindow::toggleFramelessCheck()
{
    toggleFrameless();
}

void hudWindow::toggleBackgroundCheck()
{
    toggleBackground();
}




QString hudWindow::getWindowSettings()
{
    //Main Window Settings
    // x,y,width,height,opacity,visible
    QString temp = ""
            + QString::number(pos().x()) + ","
            + QString::number(pos().y()) + ","
            + QString::number(width()) + ","
            + QString::number(height()) + ","
            + QString::number(windowOpacity()) + ","
            + ((isVisible()) ? "1" : "0" ) + ","
            + (( windowFlags()&Qt::WindowTransparentForInput ) ? "1" : "0" ) + ","
            + ( ( testAttribute(Qt::WA_NoSystemBackground) ) ? "1" : "0" );

    return temp;

}
void hudWindow::setWindowSettings(QString settings)
{

    QRegExp rx("([^,]*),([^,]*),([^,]*),([^,]*),([^,]*),([^,]*),([^,]*),([^,]*)", Qt::CaseSensitive);

    if ( rx.indexIn(settings,0) >= 0 )
    {


        setGeometry(rx.cap(1).toInt(),rx.cap(2).toInt(),std::max(rx.cap(3).toInt(),100),std::max(rx.cap(4).toInt(),75));
        setOpacity(rx.cap(5).toFloat());



        if ( rx.cap(7) == "1" )
        {
             setWindowFlags(windowFlags()|Qt::WindowTransparentForInput);
        }
        else
        {
             setWindowFlags(windowFlags()&~Qt::WindowTransparentForInput);
        }

        if ( rx.cap(6) == "1" )
        {
            show();
        }
        else
        {
            hide();
        }
        if ( rx.cap(8) == "1" )
        {
            toggleBackground(true);
        }



    }

}





bool hudWindow::toggleBackground()
{
    if ( testAttribute(Qt::WA_NoSystemBackground)  || testAttribute(Qt::WA_TranslucentBackground) )
    {
        return toggleBackground(false);
    }
    else
    {
        return toggleBackground(true);
    }
}

bool hudWindow::toggleBackground(bool state)
{
    if ( state )
    {
        setAttribute(Qt::WA_NoSystemBackground,true);
        setAttribute(Qt::WA_TranslucentBackground,true);
        show();
        return true;
    }
    else
    {  
        setAttribute(Qt::WA_NoSystemBackground,false);
        setAttribute(Qt::WA_TranslucentBackground,false);
        show();
        return false;
    }


    //setStyleSheet("background-color: rgba(0,0,0,0)");

}
