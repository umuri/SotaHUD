#ifndef HUDEXP_H
#define HUDEXP_H

#include <QWidget>
#include "explookup.h"
#include "hudwindow.h"


namespace Ui {
    class hudExp;
}

class hudExp : public hudWindow
{

    Q_OBJECT

public:
    explicit hudExp(QWidget *parent = 0);
    ~hudExp();
    void registerKill(QString name, QString target);
    void setAdvLevel(int tempAdvLevel);

private slots:
    void updateExp(int value, int secspassed);
    void on_btnExpMode_clicked();
    void on_btnExpResetCurrent_clicked();
    void on_btnExpSaveLibrary_clicked();


private:
    void help(QPoint pos);
    int advLevel = -1;
    Ui::hudExp *ui;
    explookup expLookupModel;

};

#endif // HUDEXP_H
