#include "huddecogrid.h"
#include "ui_huddecogrid.h"
#include <QDesktopWidget>

hudDecoGrid::hudDecoGrid(QWidget *parent) :
    hudWindow(parent),
    ui(new Ui::hudDecoGrid)
{
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TranslucentBackground);

    ui->setupUi(this);


    showCloseButton=false;
    showSettingsButton=false;
    showHelpButton=false;
    showPinButton=false;

    toggleClickThrough(true);
    toggleOnTop(true);

    setStyleSheet("background-color: rgba(0,0,0,0)");
    ui->gviewGrid->setStyleSheet("background-color: rgba(0,0,0,0)");




    QDesktopWidget widget;
    QRect mainScreenSize = widget.availableGeometry(widget.primaryScreen()); // or screenGeometry(), depending on your need
    setGeometry(0,0, mainScreenSize.width(), mainScreenSize.height());


    gridView = ui->gviewGrid;
    gridScene = new QGraphicsScene();

    gridView->setScene(gridScene);
    setOpacity(0.3);
    QPen redPen = QPen(Qt::red, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    QPen greenPen = QPen(Qt::green, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    for ( int i = 0; i < width(); i += 25 )
    {
        if ( i%125 == 0 )
        {
            gridScene->addLine(i,0,i,height(),redPen);
        }
        else
        {
            gridScene->addLine(i,0,i,height(),greenPen);
        }

    }
    for ( int j = 0; j < height(); j += 25)
    {
        if ( j%125 == 0 )
        {
            gridScene->addLine(0,j,width(),j,redPen);
        }
        else
        {
            gridScene->addLine(0,j,width(),j,greenPen);
        }
    }
    gridView->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    gridView->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );


}

hudDecoGrid::~hudDecoGrid()
{
    delete ui;
}
