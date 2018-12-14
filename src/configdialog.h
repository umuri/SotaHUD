#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H
#include <QDialog>
#include "config.h"
#include <QAbstractButton>

extern const int MajorVersion;
extern const int MinorVersion;
extern const int RevisionVersion;

namespace Ui {
class ConfigDialog;
}

class ConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigDialog(Config * configPass, QWidget *parent = 0);
    ~ConfigDialog();

private:
       Config * currentConfig;

private slots:
    void on_btnChangeAppDataPath_clicked();
    void on_buttonBox_accepted();

    void on_chkPastDay_toggled(bool checked);


    void on_chkUseWhitelist_toggled(bool checked);

    void on_btnAddLoginList_clicked();

    void on_btnRemoveLoginList_clicked();

    void on_buttonBox_clicked(QAbstractButton *button);

private:
    Ui::ConfigDialog *ui;
    void addCharactersToDropDown(QString path);
};

#endif // CONFIGDIALOG_H
