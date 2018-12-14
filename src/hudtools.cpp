#include "hudtools.h"
#include "ui_hudtools.h"

hudTools::hudTools(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::hudTools)
{
    ui->setupUi(this);
}

hudTools::~hudTools()
{
    delete ui;
}



