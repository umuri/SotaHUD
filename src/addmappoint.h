#ifndef ADDMAPPOINT_H
#define ADDMAPPOINT_H

#include <QDialog>

namespace Ui {
class AddMapPoint;
}

class AddMapPoint : public QDialog
{
    Q_OBJECT

public:
    explicit AddMapPoint(QWidget *parent = 0);
    QString getText();
    bool getShowText();
    QString getImage();
    double getScale();
    int getFontSize();
    ~AddMapPoint();

private:
    Ui::AddMapPoint *ui;
};

#endif // ADDMAPPOINT_H
