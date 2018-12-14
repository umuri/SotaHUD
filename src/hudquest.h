#ifndef HUDQUEST_H
#define HUDQUEST_H

#include <QWidget>
#include "hudwindow.h"
#include "questspluscontroller.h"
#include "hudquestsmodel.h"


namespace Ui {
class hudQuest;
}

class hudQuest : public hudWindow
{
    Q_OBJECT

public:
    explicit hudQuest(QWidget *parent = 0, questsPlusController * = NULL);

    ~hudQuest();

    void dataChanged();
private:
    hudQuestsModel * questModel;
    Ui::hudQuest *ui;
    questsPlusController * questController;
    void help(QPoint);
};

#endif // HUDQUEST_H
