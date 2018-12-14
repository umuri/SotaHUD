#include "configdialog.h"
#include "ui_configdialog.h"
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QRegExp>
#include <QDebug>
#include <QFileDialog>

ConfigDialog::ConfigDialog(Config * configtemp, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigDialog)
{
    ui->setupUi(this);
    currentConfig = configtemp;

    connect(ui->txtLoginListInput, SIGNAL(returnPressed()),ui->btnAddLoginList,SIGNAL(clicked()));

    addCharactersToDropDown(currentConfig->appDataPath);


    ui->txtLogDateTime->setText(currentConfig->LogDateTimeFormat);
    ui->txtFileDate->setText(currentConfig->FileDateFormat);
    ui->chkCriticalHits->setChecked( currentConfig->ShowCriticalHitsSeperate );
    ui->chkKillingBlow->setChecked( currentConfig->ShowKillingBlowSkillUsed );
    ui->chkCurrentDay->setChecked( currentConfig->ParseCurrentDay );
    ui->chkPastDay->setChecked( currentConfig->ParsePastDay );
    ui->txtLinesToClear->setText( QString::number(currentConfig->MaxClearLength) );
    ui->txtMaxDisplay->setText( QString::number(currentConfig->MaxDisplay) );
    ui->chkStartOnTop->setChecked (currentConfig->StartOnTop );
    ui->chkSalesSound->setChecked( currentConfig->SalesSound );
    ui->chkSalesPopup->setChecked( currentConfig->SalesPopup );
    ui->spnSalesTime->setValue( currentConfig->SalesTime );
    ui->chkWhisperSound->setChecked( currentConfig->WhisperSound );
    ui->chkWhisperPopup->setChecked( currentConfig->WhisperPopup );
    ui->spnWhisperTime->setValue( currentConfig->WhisperTime );
    ui->chkLogonLogoffSound->setChecked( currentConfig->LogonLogoffSound );
    ui->chkLogonLogoffPopup->setChecked( currentConfig->LogonLogoffPopup );
    ui->spnLogonLogoffTime->setValue( currentConfig->LogonLogoffTime );

    ui->sldVolume->setValue(currentConfig->Volume);
    ui->chkTrackPlayerOnStart->setChecked(currentConfig->StartTrackPlayer);
    ui->spnEventHours->setValue(currentConfig->ShowEventsHappeningInXHours);

    if ( ui->chkPastDay->isChecked() )
    {
        ui->dateStart->setDate( currentConfig->pastDayStart.date());
        ui->dateEnd->setDate( currentConfig->pastDayEnd.date());
    }
    else
    {
        ui->dateStart->setDate( QDate::currentDate() );
        ui->dateEnd->setDate( QDate::currentDate() );
        ui->dateStart->setEnabled(false);
        ui->dateEnd->setEnabled(false);
    }

    ui->cmbCharName->setCurrentIndex(ui->cmbCharName->findText( currentConfig->CharacterName));

    ui->lblAppDataPath->setText(currentConfig->appDataPath);

    ui->lstLoginList->clear();

    for ( int i = 0; i < currentConfig->loginNotificationList.count(); ++i)
    {
        ui->lstLoginList->addItem(currentConfig->loginNotificationList[i]);
    }
    ui->chkUseWhitelist->setChecked(currentConfig->loginNotificationListOn);

    //This Prevents enter from firing the dialog
    disconnect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));

}

ConfigDialog::~ConfigDialog()
{
    delete ui;
}


void ConfigDialog::addCharactersToDropDown(QString appDataPath)
{

    QString pathTempPrefix = appDataPath;
    pathTempPrefix = pathTempPrefix + "ChatLogs/";

    QDir chatLogFolder(pathTempPrefix);
    QString parseNameTemp;
    bool found = false;
    ui->cmbCharName->clear();

    for ( unsigned int i = 0; i < chatLogFolder.count(); i++)
    {
        //SotAChatLog_Umuri Maxwell_2015-08-04
        QRegExp rx("SotAChatLog_(.*)_[-0-9]*.txt", Qt::CaseInsensitive);

        if ( rx.indexIn(chatLogFolder.entryList()[i],0) < 0)
        {
           //We didn't find shit
        }
        else
        {

            parseNameTemp = rx.cap(1);
            found = false;
            for ( int j = 0; j < ui->cmbCharName->count(); j++ )
            {
                if ( parseNameTemp == ui->cmbCharName->itemText(j) )
                {
                    found = true;
                }
            }
            if ( !found )
            {
                ui->cmbCharName->addItem(parseNameTemp,parseNameTemp);
            }



        }


    }


}

void ConfigDialog::on_buttonBox_accepted()
{
    //Do nothing because we handle it all in the click event;
}

void ConfigDialog::on_buttonBox_clicked(QAbstractButton *button)
{
	if ( ( button->text() == "OK" ) || ( button->text() == "&OK" ) )
    {
    //Update the config
    currentConfig->CharacterName = ui->cmbCharName->currentText();
    currentConfig->DefaultSavePath = ui->txtDefaultSave->text();
    currentConfig->MaxClearLength = ui->txtLinesToClear->text().toInt();
    currentConfig->MaxDisplay = ui->txtMaxDisplay->text().toInt();
    currentConfig->StartOnTop = ui->chkStartOnTop->isChecked();
    currentConfig->ParseCurrentDay = ui->chkCurrentDay->isChecked();
    currentConfig->ParsePastDay = ui->chkPastDay->isChecked();
    currentConfig->pastDayStart = ui->dateStart->dateTime();
    currentConfig->pastDayEnd = ui->dateEnd->dateTime();
    currentConfig->SalesSound = ui->chkSalesSound->isChecked();
    currentConfig->SalesPopup = ui->chkSalesPopup->isChecked();
    currentConfig->SalesTime = ui->spnSalesTime->value();
    currentConfig->WhisperSound = ui->chkWhisperSound->isChecked();
    currentConfig->WhisperPopup = ui->chkWhisperPopup->isChecked();
    currentConfig->WhisperTime = ui->spnWhisperTime->value();
    currentConfig->LogonLogoffSound = ui->chkLogonLogoffSound->isChecked();
    currentConfig->LogonLogoffPopup = ui->chkLogonLogoffPopup->isChecked();
    currentConfig->LogonLogoffTime = ui->spnLogonLogoffTime->value();
    currentConfig->StartTrackPlayer = ui->chkTrackPlayerOnStart->isChecked();
    currentConfig->Volume = ui->sldVolume->value();
    currentConfig->ShowEventsHappeningInXHours = ui->spnEventHours->value();
    currentConfig->FileDateFormat = ui->txtFileDate->text();
    currentConfig->LogDateTimeFormat = ui->txtLogDateTime->text();

    currentConfig->ShowCriticalHitsSeperate = ui->chkCriticalHits->isChecked();
    currentConfig->ShowKillingBlowSkillUsed = ui->chkKillingBlow->isChecked();
    if ( currentConfig->appDataPath != ui->lblAppDataPath->text() )
    {
        currentConfig->appDataPath = ui->lblAppDataPath->text();
        currentConfig->appDataPathChanged = true;
    }


    currentConfig->loginNotificationList.clear();
    for ( int i = 0; i < ui->lstLoginList->count(); ++i)
    {
        currentConfig->loginNotificationList.append(ui->lstLoginList->item(i)->text());
    }
    currentConfig->loginNotificationListOn = ui->chkUseWhitelist->isChecked();

    currentConfig->saveSettings();

    accept();
    }
}

void ConfigDialog::on_chkPastDay_toggled(bool checked)
{

    if ( checked )
    {
        ui->dateStart->setEnabled(true);
        ui->dateEnd->setEnabled(true);
    }
    else
    {
        ui->dateStart->setEnabled(false);
        ui->dateEnd->setEnabled(false);
    }
}

void ConfigDialog::on_btnChangeAppDataPath_clicked()
{

    QFileDialog *fd = new QFileDialog;
    fd->setFileMode(QFileDialog::Directory);
    fd->setOption(QFileDialog::ShowDirsOnly);
    fd->setViewMode(QFileDialog::Detail);
    QString result = fd->getExistingDirectory(0,"Find your Shroud of the Avatar Appdata Directory",currentConfig->appDataPath);
    QString directory;
    if (result.length() > 0)
    {

        if ( result.contains("ChatLogs"))
        {
            result = result.left(result.indexOf("ChatLogs"));
        }
        else
        {
            result = result + "/";
        }


        ui->lblAppDataPath->setText(result);
        addCharactersToDropDown(result);
        if ( ui->cmbCharName->findText(currentConfig->CharacterName) >= 0 )
        {
            ui->cmbCharName->setCurrentIndex(ui->cmbCharName->findText(currentConfig->CharacterName));
        }

    }

}

void ConfigDialog::on_chkUseWhitelist_toggled(bool checked)
{
    ui->grpLoginList->setEnabled(checked);
}

void ConfigDialog::on_btnAddLoginList_clicked()
{
    ui->lstLoginList->addItem(ui->txtLoginListInput->text());
    ui->txtLoginListInput->clear();
}

void ConfigDialog::on_btnRemoveLoginList_clicked()
{
    foreach(QListWidgetItem * itemToRemove, ui->lstLoginList->selectedItems())
    {
        //Apparently the listwidget prefers you delete or takeitem, and we just want it gone, so delete
        delete itemToRemove;

    }
}


