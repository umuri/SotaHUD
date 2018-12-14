#ifndef HUDDECOGRID_H
#define HUDDECOGRID_H

#include <QWidget>
#include <hudwindow.h>
#include <QGraphicsView>
namespace Ui {
class hudDecoGrid;
}

class hudDecoGrid : public hudWindow
{
    Q_OBJECT

public:
    explicit hudDecoGrid(QWidget *parent = 0);
    ~hudDecoGrid();

private:
    Ui::hudDecoGrid *ui;
    QGraphicsView * gridView;
    QGraphicsScene * gridScene;
};

#endif // HUDDECOGRID_H
