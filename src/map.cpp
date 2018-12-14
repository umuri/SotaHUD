#include "map.h"
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <QDir>
#include <QGraphicsView>
#include <QGraphicsTextItem>
#include <QDebug>
#include <QObject>
#include <QFileDialog>

map::map(QGraphicsView * mapViewTemp)
{

    mapView = mapViewTemp;
    mapScene = new QGraphicsScene();
    QString imagePath =  QDir::currentPath() +"/icons/PlayerTracker.png";
    QImage image(imagePath);
    currentPlayerIcon = new QGraphicsPixmapItem( QPixmap::fromImage(image));
    currentPlayerIcon->setScale(0.12);
    setCurrentPlayerPosition(0,0);


    mapView->setScene(mapScene);
    loadMapList();
    loadMap(mapList[0]);






}





QString map::getMapName()
{
    return mapName;
}
QString map::getUnityName()
{
    return unityName;
}

void map::zoomIn()
{
    mapView->scale(1.2,1.2);
}
void map::zoomOut()
{
    mapView->scale(0.8,0.8);
}

void map::loadMapList()
{
    QString pathTemp;
    mapList.clear();
    pathTemp = QDir::currentPath() + "/maps/";

    QDir mapFolder(pathTemp);

    for ( unsigned int i = 0; i < mapFolder.count(); i++)
    {

        QRegExp rx("(.*).csv", Qt::CaseInsensitive);

        if ( rx.indexIn(mapFolder.entryList()[i],0) >= 0)
        {
            mapList.append(rx.cap(1));
        }

    }
}

bool map::loadMap(QString mapToLoad)
{



    QString mapPath = QDir::currentPath() + "/maps/" + mapToLoad + ".csv";
    QFile mapFile(mapPath);


    if ( !mapFile.exists() )
    {
        return false;
    }
    points.clear();
    mapScene->clear();
    mapView->items().clear();
    QString imagePath =  QDir::currentPath() +"/icons/PlayerTracker.png";
    QImage image(imagePath);
    currentPlayerIcon = new QGraphicsPixmapItem( QPixmap::fromImage(image));
    currentPlayerIcon->setScale(0.12);
    setCurrentPlayerPosition(0,0);

    mapFile.open(QIODevice::ReadOnly);

    QTextStream mapIn(&mapFile);

    QString line = mapIn.readLine();

    //First line is map data
    //Area name(string),UnityLocationName(string),orientation(int), x-scale(float), y-scale(float), x-offset(signed int),y-offset(signed int)
    //       1                   2                       3           4                5               6                   7
    QRegExp rx("([^,]*),([^,]*),([^,]*),([^,]*),([^,]*),([^,]*),([^,]*)", Qt::CaseSensitive);


    if ( rx.indexIn(line,0) >= 0 )
    {
         mapName = rx.cap(1);
         unityName = rx.cap(2);
         orientation = rx.cap(3).toInt();
         xscale = rx.cap(4).toFloat();
         yscale = rx.cap(5).toFloat();
         xoffset = rx.cap(6).toFloat();
         yoffset = rx.cap(7).toFloat();
    }
    //qDebug() << xscale << yscale << xoffset << yoffset;

    originalBackground = QImage(QDir::currentPath() + "/icons/" + mapName + "-background.png");
    if ( originalBackground.isNull())
    {
        //qDebug() << "Well crap";
        originalBackground = QImage(QDir::currentPath() + "/icons/Blank-background.png");
        if ( originalBackground.isNull() )
        {
            //qDebug() << "Well Double Crap";
        }

    }


    background = new QGraphicsPixmapItem(QPixmap::fromImage(originalBackground.scaled(originalBackground.width()*xscale, originalBackground.height()*yscale, Qt::IgnoreAspectRatio, Qt::SmoothTransformation)));




    background->setPos(xoffset,yoffset); //position it to cover all the scene so at 0,0 which is the origin points
    background->setZValue(-5000); //to ensure it is always at the back

    mapScene->addItem(background);



    //MapEntryFormat
    //item name(String), xPos(float), yPos(float),zPos(float),ShowText(True/False),IconFileName(string),IconScaleFactor(float),FontSize(int)
    while(!mapIn.atEnd())
    {

       line = mapIn.readLine();
       QRegExp rx("([^,]*),([^,]*),([^,]*),(.*)", Qt::CaseSensitive);


       if ( rx.indexIn(line,0) >= 0 )
       {

         mappoint t;
         t.text = rx.cap(1);
         t.x = rx.cap(2).toDouble();
         t.y = rx.cap(3).toDouble();
         if ( rx.cap(4).contains(",") ) //We have more matches, optional arguments!
         {

            //Process this argument, which is y location
            QString processed = rx.cap(4);
            t.z = processed.left(processed.indexOf(",")).toDouble();
            processed = processed.mid(processed.indexOf(",")+1); //throw out the last argument;
            if ( processed.contains(",") )  //Still another argument left
            {
                //This argument is the image path
                if ( processed.left(processed.indexOf(",")) == "true" )
                {
                    t.showText = true;
                }
                else
                {
                    t.showText = false;
                }

                processed = processed.mid(processed.indexOf(",")+1); //throw out the last argument;
                if ( processed.contains(",") )  //Still another argument left
                {
                    t.image = processed.left(processed.indexOf(","));

                    processed = processed.mid(processed.indexOf(",")+1); //throw out the last argument;

                    if ( processed.contains(",") )  //Still another argument left
                    {
                        t.imageScale = processed.left(processed.indexOf(",")).toDouble();
                        processed = processed.mid(processed.indexOf(",")+1); //throw out the last argument;
                        t.fontSize = processed.toInt();

                    }
                    else
                    {
                        t.imageScale = processed.toDouble();
                        t.fontSize=12;
                    }

                }
                else
                {
                    t.image = processed;
                    t.imageScale=1;
                    t.fontSize=12;
                }
            }
            else
            {
                if ( processed == "true" )
                {
                    t.showText = true;
                }
                else
                {
                    t.showText = false;
                }
                t.image = t.text;
                t.imageScale=1;
                t.fontSize=12;
            }
         }
         else
         {
             t.z = rx.cap(4).toDouble();
             t.image = t.text;
             t.showText = true;
             t.imageScale=1;
             t.fontSize=12;
         }



         //qDebug() << t.x << "," << t.y << "," << t.z << "   " << t.text << "   " << t.showText << "   " << t.image << "   " << t.imageScale;

         //Load the map item
         //qDebug() << rx.cap(1) << ":" <<  rx.cap(2) << "," << rx.cap(3) << "," << rx.cap(4);

         points.append(t);
         //What a pretty image! actually it's a circle
         //mapScene->addEllipse(t.x,t.y,10,10);







       }
    }



    for ( int i = 0; i < points.size(); i++ )
    {
        drawMapPoint(&points[i]);


    }

    mapScene->addItem(currentPlayerIcon);
    mapScene->setSceneRect(mapScene->itemsBoundingRect());
    mapView->show();


    return true;
}


void map::drawMapPoint(mappoint * pointToDraw)
{

    double drawX;
    double drawY;
    if ( orientation == 0 )
    {
        drawX = pointToDraw->x;
        drawY = -pointToDraw->z;
    }
    else if ( orientation == 90 )
    {
        drawX = pointToDraw->x;
        drawY = -pointToDraw->z;
    }
    else if ( orientation == -90 )
    {
        drawX = pointToDraw->x;
        drawY = -pointToDraw->z;
    }
    else
    {
        drawX = pointToDraw->x;
        drawY = pointToDraw->z;
    }



    if ( pointToDraw->image.length() > 0 )
    {
        QString imagePath =  QDir::currentPath() +"/icons/" + pointToDraw->image + ".png";
        QImage image(imagePath);
        if ( image.isNull() )
        {

        }
        else
        {
        QGraphicsPixmapItem * item = new QGraphicsPixmapItem( QPixmap::fromImage(image));
        item->setScale(pointToDraw->imageScale);
        item->setPos(drawX, drawY);

        mapScene->addItem(item);
        }
    }

    if ( pointToDraw->showText )
    {
        QGraphicsTextItem * io = new QGraphicsTextItem;
        io->setPos(drawX-(pointToDraw->text.length()*4), drawY+9);
        io->setPlainText(pointToDraw->text);
        io->setFont(QFont("Courier", pointToDraw->fontSize, QFont::Bold, true));
       mapScene->addItem(io);
    }
}

void map::createMap(QString newMapName, QString newUnityName, int newOrientation, double x, double /* y */, double z)
{
    mapName = newMapName;
    unityName = newUnityName;
    orientation = newOrientation;
    xscale = 5;
    yscale = 5;
    xoffset = -500;
    yoffset = -500;
    points.clear();

    mappoint center;

    center.showText = true;
    center.imageScale = 1;
    center.x = 0;
    center.y = 0;
    center.z = 0;
    center.image = "";
    center.text="0-0";
    center.fontSize=8;
    points.append(center);

    mapScene->clear();
    mapView->items().clear();


    QGraphicsTextItem * io = new QGraphicsTextItem;
    io->setPos(center.x,center.z+7);
    io->setPlainText(center.text);
    io->setFont(QFont("Courier", center.fontSize, QFont::Bold, true));
    mapScene->addItem(io);


    QString imagePath =  QDir::currentPath() +"/icons/PlayerTracker.png";
    QImage image(imagePath);
    currentPlayerIcon = new QGraphicsPixmapItem( QPixmap::fromImage(image));
    currentPlayerIcon->setScale(0.12);
    setCurrentPlayerPosition(x,z);



    originalBackground =  QImage(QDir::currentPath() + "/icons/Blank-background.png");
    background = new QGraphicsPixmapItem(QPixmap::fromImage(originalBackground.scaled(originalBackground.width()*xscale, originalBackground.height()*yscale, Qt::IgnoreAspectRatio, Qt::SmoothTransformation)));
    background->setPos(xoffset,yoffset); //position it to cover all the scene so at 0,0 which is the origin point
    background->setZValue(-5000); //to ensure it is always at the back
    mapScene->addItem(background);





    mapScene->addItem(currentPlayerIcon);
    mapScene->setSceneRect(mapScene->itemsBoundingRect());



}

void map::setBackgroundScaleX(int newscale)
{
    xscale = ((float)newscale)/100;
    background->setPixmap(QPixmap::fromImage(originalBackground.scaled(originalBackground.width()*xscale, originalBackground.height()*yscale, Qt::IgnoreAspectRatio, Qt::SmoothTransformation)));
}

void map::setBackgroundScaleY(int newscale)
{
    yscale = ((float)newscale)/100;
    background->setPixmap(QPixmap::fromImage(originalBackground.scaled(originalBackground.width()*xscale, originalBackground.height()*yscale, Qt::IgnoreAspectRatio, Qt::SmoothTransformation)));
}

void map::setBackgroundX(int newoffset)
{
    xoffset=newoffset;
    background->setPos(xoffset,yoffset);
}

void map::setBackgroundY(int newoffset)
{
    yoffset=newoffset;
    background->setPos(xoffset,yoffset);
}


void map::trackPlayerTimerEvent(double x, double y, double z, bool moveWindow)
{

    if ( tracePath ) //Draw a line!
    {
        tracePathLine(x,y,z,curX,curY,curZ);
    }

    //qDebug() << x << "," << y << "," << z;
    curX = x;
    curY = y;
    curZ = z;


    setCurrentPlayerPosition(x,z);
    if ( moveWindow )
    {
        mapView->centerOn(currentPlayerIcon->pos());
    }

}


void map::tracePathLine(double x, double /* y */, double z, double oldx, double /* oldy */, double oldz)
{
    double tx;
    //double ty; //Also not used because it's elevation
    double tz;
    double toldx;
    //double toldy; //Not used because i'm horrible and we don't actually care about elevation
    double toldz;

    if ( orientation == 0 )
    {
        tx = x;
        toldx = oldx;
        //ty = y;
        //toldy = oldy;
        tz = -z;
        toldz = -oldz;
    }
    else if ( orientation == 90 )
    {
        tx = x;
        toldx = oldx;
        //ty = y;
        //toldy = oldy;
        tz = -z;
        toldz = -oldz;
    }
    else if ( orientation == -90 )
    {
        tx = x;
        toldx = oldx;
        //ty = y;
        //toldy = oldy;
        tz = -z;
        toldz = -oldz;;
    }
    else
    {
        tx = x;
        toldx = oldx;
        //ty = y;
        //toldy = oldy;
        tz = -z;
        toldz = -oldz;
    }



    mapScene->addLine(tx,tz,toldx,toldz,QPen(Qt::black, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
}

void map::setCurrentPlayerPosition(double tempX, double tempZ)
{
    double drawX;
    double drawY;
    if ( orientation == 0 )
    {
        drawX = tempX;
        drawY = -tempZ;
    }
    else if ( orientation == 90 )
    {
        drawX = tempX;
        drawY = -tempZ;
    }
    else if ( orientation == -90 )
    {
        drawX = tempX;
        drawY = -tempZ;
    }
    else
    {
        drawX = tempX;
        drawY = tempZ;
    }

    currentPlayerIcon->setPos(drawX,drawY);

}


void map::saveMap()
{
    QString saveFileName = QFileDialog::getSaveFileName(NULL,"Save the map file for" + mapName,  QDir::currentPath() + "/maps/" + mapName + ".csv", "*.csv" );
    if ( saveFileName != NULL )
    {
        QFile f( saveFileName );
        f.open( QIODevice::WriteOnly );

        QTextStream out(&f);

        //First line is map data
        //Area name(string),UnityLocationName(string),orientation(int), x-scale(float), y-scale(float), x-offset(signed int),y-offset(signed int)
        //       1                   2                       3           4                5               6                   7
        out << mapName << ","
            << unityName  << ","
            << QString::number(orientation)  << ","
            << QString::number(xscale)  << ","
            << QString::number(yscale)  << ","
            << QString::number(xoffset)  << ","
            << QString::number(yoffset) << endl;


        for ( int i = 0; i < points.size(); i++ )
        {
        //MapEntryFormat
        //item name(String), xPos(float), yPos(float),zPos(float),ShowText(True/False),IconFileName(string),IconScaleFactor(float)
            out << points[i].text << ","
                << QString::number(points[i].x) << ","
                << QString::number(points[i].y) << ","
                << QString::number(points[i].z) << ","
                << (( points[i].showText) ?  "true" : "false" ) << ","
                << points[i].image << ","
                << QString::number(points[i].imageScale) << ","
                << QString::number(points[i].fontSize)  << endl;
        }
        f.close();
    }
}


void map::addPoint(QString tempText, bool tempShowText, QString tempImage, float tempImageScale , int tempFontSize)
{
    mappoint newpoint;
    newpoint.text = tempText;
    newpoint.image = tempImage;
    newpoint.showText = tempShowText;
    newpoint.imageScale = tempImageScale;
    newpoint.x = curX;
    newpoint.y = curY;
    newpoint.z = curZ;
    newpoint.fontSize = tempFontSize;

    drawMapPoint(&newpoint);
    points.append(newpoint);

}

void map::startPathTrace()
{
 tracePath = true;
}
void map::endPathTrace()
{
 tracePath = false;

}

void map::saveImage()
{
    QImage image(mapScene->sceneRect().size().toSize(), QImage::Format_ARGB32);
    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing);
    mapScene->render(&painter);
    image.save(QDir::currentPath() + "/icons/" + mapName + "-background.png");

    //Anything that calls this should reset the scale of the map and save it, since it's now 1:1

}



int map::getBackgroundX()
{
    return xoffset;
}
int map::getBackgroundY()
{
    return yoffset;
}
float map::getBackgroundScaleX()
{
    return xscale;
}

float map::getBackgroundScaleY()
{
    return yscale;
}

float map::getBackgroundImageX()
{
    return background->boundingRect().width();
}

float map::getBackgroundImageY()
{
    return background->boundingRect().height();
}
