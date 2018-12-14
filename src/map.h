#ifndef MAP_H
#define MAP_H
#include <QString>
#include <QList>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QTimer>


struct mappoint {
    QString text;
    double x;
    double y;
    double z;
    bool showText = false;
    QString image;
    float imageScale;
    int fontSize;

};

class map
{


public:
    map(QGraphicsView * mapCanvasTemp);


    bool loadMap(QString mapToLoad);
    void setTracking(bool checked);
    void zoomIn();
    void zoomOut();
    QList<QString> mapList;
    void trackPlayerTimerEvent(double x, double y, double z, bool moveWindow);

    void createMap(QString newMapName, QString newUnityName, int newOrientation, double x, double y, double z);
    void setBackgroundScaleX(int newscale);
    void setBackgroundScaleY(int newscale);
    void setBackgroundX(int);
    void setBackgroundY(int);
    void startPathTrace();

    void endPathTrace();
    void saveImage();
    void saveMap();
    QString getMapName();
    QString getUnityName();
    void addPoint(QString tempText, bool tempShowText, QString tempImage, float tempImageScale, int tempFontSize );
    int getBackgroundX();
    int getBackgroundY();
    float getBackgroundScaleX();
    float getBackgroundScaleY();


    float getBackgroundImageY();
    float getBackgroundImageX();
private:

    double curX=0;
    double curY=0;
    double curZ=0;

    double xscale = 1;
    double yscale = 1;
    double xoffset = 0;
    double yoffset = 0;
    bool tracePath = false;
    QString mapName;
    QString unityName;
    int orientation;
    void loadMapList();
    QGraphicsView * mapView;
    QGraphicsScene * mapScene;
    QList<mappoint> points;
    void drawMapPoint(mappoint * pointToDraw);
    QGraphicsPixmapItem * currentPlayerIcon;
    QImage originalBackground;
    QGraphicsPixmapItem * background;
    void setCurrentPlayerPosition(double x, double y);
    void tracePathLine(double x, double y, double z, double oldx, double oldy, double oldz);


};

#endif // MAP_H
