#include "addmappoint.h"
#include "ui_addmappoint.h"

AddMapPoint::AddMapPoint(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddMapPoint)
{
    ui->setupUi(this);
}

AddMapPoint::~AddMapPoint()
{
    delete ui;
}

bool AddMapPoint::getShowText()
{
    return ui->chkShowText->checkState();
}

QString AddMapPoint::getImage()
{
    return ui->txtImage->text();
}

double AddMapPoint::getScale()
{
    return ((double)ui->sldScale->value())/100;
}
int AddMapPoint::getFontSize()
{
    return ui->spnFontSize->value();
}

QString AddMapPoint::getText()
{
    return ui->txtPointName->text();
}
