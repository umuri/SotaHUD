#ifndef HUDMAP_H
#define HUDMAP_H

#include <QWidget>
#include "map.h"
#include "hudwindow.h"

namespace Ui {
    class hudMap;
}

class hudMap : public hudWindow
{
    Q_OBJECT

public:
    explicit hudMap(QWidget *parent = 0);
    ~hudMap();

    void hudMapTrackPlayerTimerEvent(QString mapName, QString unityName, int orientation, double x, double y, double z);

    void setTrackedPlayerChecked(bool state);




protected:
    QPushButton * zoomIn;
    QPushButton * zoomOut;

    void resizeEvent(QResizeEvent *);

private:
    map * currentMap;
    Ui::hudMap *ui;

    void updateSlidersFromMap();
private slots:

    void on_btnReload_clicked();

    void zoomInClicked();

    void zoomOutClicked();


    void on_chkTrackPlayer_toggled(bool checked);



    void on_chkAdvancedMapOptions_toggled(bool checked);

    void on_btnMapSave_clicked();

    void on_btnMapAddPoint_clicked();

    void on_btnMapSaveImage_clicked();

    void on_btnMapTracePath_clicked();

    void toggleEditTools();
    void on_sldBackgroundX_sliderMoved(int position);
    void on_sldBackgroundY_sliderMoved(int position);
    void on_sldBackgroundScaleX_sliderMoved(int position);
    void on_sldBackgroundScaleY_sliderMoved(int position);
    void on_txtBackX_editingFinished();
    void on_txtBackY_editingFinished();
    void on_txtBackScaleX_editingFinished();
    void on_txtBackScaleY_editingFinished();
};

#endif // HUDMAP_H
