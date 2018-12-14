#ifndef HUDTOOLS_H
#define HUDTOOLS_H

#include <QWidget>

namespace Ui {
class hudTools;
}

class hudTools : public QWidget
{
    Q_OBJECT

public:
    explicit hudTools(QWidget *parent = 0);
    ~hudTools();

private:
    Ui::hudTools *ui;
};

#endif // HUDTOOLS_H
