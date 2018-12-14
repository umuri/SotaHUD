#ifndef HUDWINDOW_H
#define HUDWINDOW_H

#include <QWidget>
#include <QAction>
#include <QList>
#include <QPushButton>

class hudWindow : public QWidget
{
    Q_OBJECT


public:
    hudWindow();
    hudWindow(QWidget * temp);

    bool toggleOnTop();
    bool toggleOnTop(bool state);

    bool toggleClickThrough();
    bool toggleClickThrough(bool state);

    void setOpacity(float value);
    void closeWindow();

    bool toggleFrameless();
    bool toggleFrameless(bool state);

    bool toggleBackground();
    bool toggleBackground(bool state);

    QString getWindowSettings();
    void setWindowSettings(QString settings);

    bool showCloseButton = true;
    bool showPinButton = true;
    bool showSettingsButton = true;
    bool showHelpButton = true;



    bool isFrameless();
    bool isClickthrough();
    bool isOnTop();
    void restore();
    void minimize();

    void finishSetup();
    void initialPosition(float x, float y);
private:
    bool minimized = false;
    QPushButton *closeButton;
    QPushButton *pinButton;
    QPushButton *settingsButton;
    QPushButton *helpButton;

    QPixmap pinnedPixMap;
    QPixmap unpinnedPixMap;





protected slots:

    void toggleFramelessCheck();
    void toggleBackgroundCheck();

protected:
    QList<QAction*> rightClickActionList;
    QList<QAction*> settingsActionList;
    QAction * framelessAct;
    QAction * toggleBackgroundAct;
    void resizeEvent(QResizeEvent *);
    void contextMenuEvent(QContextMenuEvent *event);


    void settings(QPoint Pos);
    virtual void help(QPoint pos);

private slots:
    void closeButtonClicked();
    void settingsButtonClicked();
    void helpButtonClicked();
    void pinButtonClicked();

};

#endif // HUDWINDOW_H
