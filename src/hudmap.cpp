#include "hudmap.h"
#include "ui_hudmap.h"
#include "addmappoint.h"
#include <QFile>
#include <QXmlStreamReader>
#include <QDebug>
#include <QDir>

hudMap::hudMap(QWidget *parent) :
    hudWindow(parent),
    ui(new Ui::hudMap)
{

    ui->setupUi(this);


    currentMap = new map(ui->grphMap);
    for ( int i = 0; i < currentMap->mapList.size(); i++)
    {
        ui->cmbMapList->addItem(currentMap->mapList[i]);
    }

    //Hide the edit map options
     on_chkAdvancedMapOptions_toggled(false);



     QAction * mapActBasicEdit = new QAction(tr("Edit Tools"), this);
     rightClickActionList.append(mapActBasicEdit);

     connect(mapActBasicEdit, &QAction::triggered, this, &hudMap::toggleEditTools);

     QString imagePath =  QDir::currentPath() +"/icons/";

     QPixmap zoomInPixmap(imagePath + "plus.png");
     QIcon zoomInButtonIcon(zoomInPixmap);
     zoomIn = new QPushButton(QString(""), this);
     zoomIn->setFixedSize(15,15);
     zoomIn->setIcon(zoomInButtonIcon);
     zoomIn->setIconSize(zoomInPixmap.rect().size());

     QPixmap zoomOutPixmap(imagePath + "minus.png");
     QIcon zoomOutButtonIcon(zoomOutPixmap);
     zoomOut = new QPushButton(QString(""), this);
     zoomOut->setFixedSize(15,15);
     zoomOut->setIcon(zoomOutButtonIcon);
     zoomOut->setIconSize(zoomOutPixmap.rect().size());

     connect(zoomOut, &QPushButton::clicked, this, &hudMap::zoomOutClicked);
     connect(zoomIn, &QPushButton::clicked, this, &hudMap::zoomInClicked);

    updateSlidersFromMap();

     //This must be at the end of every HudWindow constructor in order to properly setup transparent backgrounds.
    finishSetup();
 }

 void hudMap::resizeEvent( QResizeEvent* z ) {
     hudWindow::resizeEvent(z);
     int widthPlacement = width() - 15;
     zoomOut->move( widthPlacement, 25 );
     zoomIn->move( widthPlacement, 45 );
}



hudMap::~hudMap()
{
    delete ui;
}

void hudMap::setTrackedPlayerChecked(bool state)
{
    ui->chkTrackPlayer->setChecked(state);
}


void hudMap::zoomInClicked()
{
    currentMap->zoomIn();
}

void hudMap::zoomOutClicked()
{
    currentMap->zoomOut();

}


void hudMap::on_chkTrackPlayer_toggled(bool /*checked*/)
{

}


void hudMap::on_chkAdvancedMapOptions_toggled(bool checked)
{
    if ( !checked )
    {
        ui->lblBackgroundX->hide();
        ui->lblBackgroundY->hide();
        ui->lblBackgroundScaleX->hide();
        ui->sldBackgroundScaleX->hide();
        ui->lblBackgroundScaleY->hide();
        ui->sldBackgroundScaleY->hide();
        ui->sldBackgroundX->hide();
        ui->sldBackgroundY->hide();
        ui->btnMapSetBackground->hide();
        ui->btnMapAddPoint->hide();
        ui->btnMapSave->hide();
        ui->btnMapSaveImage->hide();
        ui->btnMapTracePath->hide();
        ui->txtBackScaleX->hide();
        ui->txtBackScaleY->hide();
        ui->txtBackX->hide();
        ui->txtBackY->hide();
    }
    else
    {
        ui->lblBackgroundX->show();
        ui->lblBackgroundY->show();
        ui->lblBackgroundScaleX->show();
        ui->sldBackgroundScaleX->show();
        ui->lblBackgroundScaleY->show();
        ui->sldBackgroundScaleY->show();
        ui->sldBackgroundX->show();
        ui->sldBackgroundY->show();
        ui->btnMapSetBackground->show();
        ui->btnMapAddPoint->show();
        ui->btnMapSave->show();
        ui->btnMapSaveImage->show();
        ui->btnMapTracePath->show();
        ui->txtBackScaleX->show();
        ui->txtBackScaleY->show();
        ui->txtBackX->show();
        ui->txtBackY->show();
    }
}


void hudMap::on_btnMapSave_clicked()
{
    currentMap->saveMap();
}

void hudMap::on_btnMapAddPoint_clicked()
{
    AddMapPoint ppd;

//    bool onTop = false;
//    if ( ui->actionStay_On_Top->isChecked())
//    {
//        onTop = true;
//    }

//    if ( onTop )
//    {
//        toggleOnTop(false);
//    }

    if (ppd.exec() == QDialog::Accepted) { /* The user clicked Ok */

    currentMap->addPoint(ppd.getText(),ppd.getShowText(),ppd.getImage(),ppd.getScale(),ppd.getFontSize()   );

    }

//    if ( onTop )
//    {
//        toggleOnTop(true);
//    }


}


void hudMap::on_btnMapSaveImage_clicked()
{
    currentMap->saveImage();
    currentMap->setBackgroundScaleX(100);
    currentMap->setBackgroundScaleY(100);
    currentMap->saveMap();


    ui->btnReload->click();

}

void hudMap::on_btnMapTracePath_clicked()
{
    if ( ui->btnMapTracePath->text() == "Start Path Trace" )
    {
        ui->btnMapTracePath->setText("End Path Trace");
        currentMap->startPathTrace();
    }
    else
    {
        ui->btnMapTracePath->setText("Start Path Trace");
        currentMap->endPathTrace();
    }

}



void hudMap::hudMapTrackPlayerTimerEvent( QString mapName, QString unityName, int orientation, double x,double y,double z)
{


    if ( ui->chkTrackPlayer->isChecked() )
    {
        if (( currentMap->getMapName() != mapName ) || ( currentMap->getUnityName() != unityName ))
        {

            if ( ui->cmbMapList->findText(mapName) )
            {
                ui->cmbMapList->setCurrentIndex(ui->cmbMapList->findText(mapName));
            }

            if ( !currentMap->loadMap(mapName) )
            {

                currentMap->createMap(mapName,unityName,orientation,x,y,z);

            }
            updateSlidersFromMap();
        }
    }



    currentMap->trackPlayerTimerEvent(x,y,z, ui->chkMoveWindow->isChecked());
}

void hudMap::updateSlidersFromMap()
{
    ui->sldBackgroundScaleX->setValue(currentMap->getBackgroundScaleX()*100);
    ui->sldBackgroundScaleY->setValue(currentMap->getBackgroundScaleY()*100);
    ui->txtBackScaleX->setText(QString::number(currentMap->getBackgroundScaleX()));
    ui->txtBackScaleY->setText(QString::number(currentMap->getBackgroundScaleY()));
    ui->sldBackgroundX->setValue(currentMap->getBackgroundX());
    ui->sldBackgroundY->setValue(currentMap->getBackgroundY());
    ui->txtBackX->setText(QString::number(currentMap->getBackgroundX()));
    ui->txtBackY->setText(QString::number(currentMap->getBackgroundY()));
}

void hudMap::toggleEditTools()
{
    ui->chkAdvancedMapOptions->toggle();
    if ( ui->chkAdvancedMapOptions->isChecked() )
    {
        on_chkAdvancedMapOptions_toggled(true);
    }
    else
    {
        on_chkAdvancedMapOptions_toggled(false);
    }
}


void hudMap::on_sldBackgroundX_sliderMoved(int position)
{
    currentMap->setBackgroundX(position);
    ui->txtBackX->setText(QString::number(position));
}

void hudMap::on_sldBackgroundY_sliderMoved(int position)
{
    currentMap->setBackgroundY(position);
    ui->txtBackY->setText(QString::number(position));
}

void hudMap::on_sldBackgroundScaleX_sliderMoved(int position)
{
    currentMap->setBackgroundScaleX(position);
    ui->txtBackScaleX->setText(QString::number((float)position/100));
}
void hudMap::on_sldBackgroundScaleY_sliderMoved(int position)
{
    currentMap->setBackgroundScaleY(position);
    ui->txtBackScaleY->setText(QString::number((float)position/100));
}

void hudMap::on_btnReload_clicked()
{
    currentMap->loadMap(ui->cmbMapList->currentText());
    updateSlidersFromMap();
}





void hudMap::on_txtBackX_editingFinished()
{
    float newValue = ui->txtBackX->text().toFloat();
    currentMap->setBackgroundX(newValue);
    ui->sldBackgroundX->setValue(newValue);
}

void hudMap::on_txtBackY_editingFinished()
{
    float newValue = ui->txtBackY->text().toFloat();
    currentMap->setBackgroundY(newValue);
    ui->sldBackgroundY->setValue(newValue);
}

void hudMap::on_txtBackScaleX_editingFinished()
{
    float newValue = ui->txtBackScaleX->text().toFloat() * 100;
    currentMap->setBackgroundScaleX(newValue);
    ui->sldBackgroundScaleX->setValue(newValue);
}

void hudMap::on_txtBackScaleY_editingFinished()
{
    float newValue = ui->txtBackScaleY->text().toFloat() * 100;
    currentMap->setBackgroundScaleY(newValue);
    ui->sldBackgroundScaleY->setValue(newValue);
}
