#ifndef HUDQUESTJOURNAL_H
#define HUDQUESTJOURNAL_H

#include <QWidget>
#include <hudwindow.h>
#include <questsplusquest.h>
#include <QLabel>

namespace Ui {
class hudQuestJournal;
}

class hudQuestJournal : public hudWindow
{
    Q_OBJECT

public:
    explicit hudQuestJournal(QWidget *parent = 0);
    ~hudQuestJournal();

    void display();
    void setQuest(questsPlusQuest *questToSet);
private:
    QLabel * lblBackground;
    QLabel * lblJournalText;
    QPixmap * backgroundImage;
    QString journal;
    Ui::hudQuestJournal *ui;
    void resizeEvent(QResizeEvent *);
    void keyPressEvent(QKeyEvent *e);
};

#endif // HUDQUESTJOURNAL_H
