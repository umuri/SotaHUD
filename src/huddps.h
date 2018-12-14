#ifndef HUDDPS_H
#define HUDDPS_H

#include <QWidget>
#include <hudwindow.h>
#include <QDateTime>
#include <QTimer>

struct dpsEntry {
    QString target;
    QString weapon;
    int amount;
    QDateTime time;
};

struct dpsSource {
    QString source;
    std::vector<dpsEntry> entries;
};



class hudDPS : public hudWindow
{
public:

    Q_OBJECT



    std::vector<QString> labels;
    std::vector<double> values;

    std::vector<QRect> bars;
    std::vector<dpsSource> sources;
    QBrush brush;
    QString ylabel;



public:

    explicit hudDPS(QWidget *parent = 0);

    void paintBargraph(QPainter& painter);

    QBrush getBrush() const;
    void parseLine(QString tempLine, QDateTime timeTemp);

    void setWindowSettings(QString settings);
    QString getWindowSettings();


private:

    int margin = 2;
    double max_val = 25;
    double min_val = 0;
    double span = 0;
    int barHeight = 20;
    int labelWidth = 40;
    int SECSFORDPS = 60;

    QTimer updateTimer;
    bool updated = false;
    void paintEvent(QPaintEvent *event)override;
    void mouseMoveEvent(QMouseEvent *event)override;
    void resizeEvent(QResizeEvent *event)override;
    void recalcBasicValues();

private slots:
    void timerEvent(QTimerEvent *e);

    void setBarHeightClicked();
    void setDPSTimeClicked();
    void setLabelWidthClicked();
    void setMaxDPSClicked();
    void setMinDPSClicked();
};

#endif // HUDDPS_H

