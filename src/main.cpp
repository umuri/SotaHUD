#include "mainwindow.h"
#include <QApplication>
#include <QSplashScreen>
#include <QDebug>
#include <QDir>



const int MajorVersion = 0;
const int MinorVersion = 7;
const int RevisionVersion = 0;



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QFile f(QDir::currentPath() + "/style.css");
    if (!f.exists())
    {
        //Well no style sheet for you then.
    }
    else
    {
        f.open(QFile::ReadOnly | QFile::Text);
        QTextStream ts(&f);
        qApp->setStyleSheet(ts.readAll());
    }

    QPixmap pixmap(":/Splash.png");
    QSplashScreen *splash = new QSplashScreen(pixmap);
    splash->show();

    MainWindow w;



    splash->showMessage("Doing nothing");

    qApp->processEvents();

    splash->showMessage("Doing more nothing");

    qApp->processEvents();

    splash->close();

    //qDebug() << w.windowFlags();
    w.show();
    //qDebug() << w.windowFlags();


    return a.exec();

}


