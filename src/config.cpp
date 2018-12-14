#include "config.h"
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>
#include "configdialog.h"

Config::Config()
{

    QString AppDataPathPrefix;
    QString pathTempPrefix;
    pathTempPrefix = QStandardPaths::standardLocations(QStandardPaths::AppDataLocation)[0];
    pathTempPrefix = pathTempPrefix.left(pathTempPrefix.indexOf("Roaming")+7);
    pathTempPrefix = pathTempPrefix + "/Portalarium/Shroud of the Avatar/";


    if ( QDir(pathTempPrefix).exists()) //Windows!
    {
        AppDataPathPrefix = pathTempPrefix;
    }
    else //Linux, we hope
    {
         pathTempPrefix = QStandardPaths::standardLocations(QStandardPaths::ConfigLocation)[0];
         pathTempPrefix = pathTempPrefix + "/Portalarium/Shroud of the Avatar/";
         AppDataPathPrefix = pathTempPrefix;
    }
    appDataPath = AppDataPathPrefix;

}


int Config::loadConfig()
{
    bool foundVersion = false;
    QString configPath = QDir::currentPath() + "/settings.cfg";
    QFile configFile(configPath);

    QLocale sysLocale;
    SystemLocaleName = QLocale::system().name();

    if ( LogDateTimeFormat.length() < 2)
    {


        QString LogTimeFormat = sysLocale.timeFormat(QLocale::LongFormat);
        QString LogDateFormat = sysLocale.dateFormat(QLocale::ShortFormat);

        LogDateTimeFormat = LogDateFormat + " " + LogTimeFormat;
    }

    if ( configFile.exists() )
    {
        configFile.open(QIODevice::ReadOnly);

        QTextStream configIn(&configFile);
        QString configSetting;
        QString configValue;
        while(!configIn.atEnd())
        {
            QString line = configIn.readLine();

           QRegExp rx("([A-Z]*)=\\[(.*)\\]", Qt::CaseSensitive);

           if ( rx.indexIn(line,0) >= 0 )
           {
             configSetting=rx.cap(1);
             configValue=rx.cap(2);


            if ( configSetting == "CHARACTERNAME" )
            {
                CharacterName = configValue;
            }
            else if ( configSetting == "SHOWKILLINGBLOWSKILL")
            {
                if ( configValue.toInt() > 0 )
                {
                    ShowKillingBlowSkillUsed = true;
                }
            }
            else if ( configSetting == "SEPERATECRITICALHITS")
            {
                if ( configValue.toInt() > 0 )
                {
                    ShowCriticalHitsSeperate = true;

                }

            }
            else if ( configSetting == "STARTALWAYSONTOP" )
            {
                if ( configValue.toInt() > 0 )
                {
                    StartOnTop = true;
                }

            }
            else if ( configSetting == "PARSECURRENTDAY")
            {
                if ( configValue.toInt() > 0 )
                {
                   ParseCurrentDay = true;
                }
            }
            else if ( configSetting == "PARSEPASTDAY")
            {
                if ( configValue.toInt() > 0 )
                {
                   ParsePastDay = true;
                }
            }
            else if (( configSetting == "PARSEPASTDAYBEGIN" ) && ( ParsePastDay == true ) )
            {
                pastDayStart = QDateTime::fromString(configValue,"yyyy-MM-dd");
            }
            else if (( configSetting == "PRASEPASTDAYEND" ) && ( ParsePastDay == true ) )
            {
                pastDayEnd = QDateTime::fromString(configValue,"yyyy-MM-dd");
            }
            else if ( configSetting == "MAXDISPLAYLINES" )
            {
                MaxDisplay = configValue.toInt();
            }
            else if ( configSetting == "LINESTOCLEARWHENFULL" )
            {
                MaxClearLength = configValue.toInt();
            }
            else if ( configSetting == "DEFAULTSAVEPATH" )
            {
                if ( configValue == "")
                {
                  DefaultSavePath = QDir::currentPath();
                }
                else
                {
                    DefaultSavePath = configValue;
                }
            }
            else if ( configSetting == "SALESSOUND" )
            {
                if ( configValue.toInt() > 0 )
                {
                    SalesSound = true;
                }
            }
            else if ( configSetting == "SALESPOPUP" )
            {
                if ( configValue.toInt() > 0 )
                {
                    SalesPopup = true;
                }
            }
            else if ( configSetting == "SALESTIME" )
            {
                SalesTime = configValue.toInt();
            }
            else if ( configSetting == "WHISPERSOUND" )
            {
                if ( configValue.toInt() > 0 )
                {
                    WhisperSound = true;
                }
            }
            else if ( configSetting == "WHISPERPOPUP" )
            {
                if ( configValue.toInt() > 0 )
                {
                    WhisperPopup = true;
                }
            }
            else if ( configSetting == "WHISPERTIME" )
            {
                WhisperTime = configValue.toInt();
            }
            else if ( configSetting == "VOLUME" )
            {
                Volume = configValue.toInt();
            }
            else if ( configSetting == "SHOWEVENTSINXHOURS" )
            {
                ShowEventsHappeningInXHours = configValue.toInt();
            }
            else if ( configSetting == "STARTTRACKPLAYER" )
            {
                if ( configValue.toInt() > 0 )
                {
                    StartTrackPlayer = true;
                }
            }
            else if ( configSetting == "APPDATAPATH")
            {
                if ( configValue.length() > 3 )
                {
                    appDataPath = configValue;
                    appDataPathChanged = true;
                }

            }
            else if ( configSetting == "LOGDATETIMEFORMAT")
            {   LogDateTimeFormat = configValue;  }
            else if ( configSetting == "FILEDATEFORMAT")
            {   FileDateFormat = configValue;  }

            else if ( configSetting == "MAINWINDOWSETTINGS")   //FINDME: ADDWINDOW
            {   mainWindowSettings = configValue;  }
            else if ( configSetting == "MAPWINDOWSETTINGS")
            {   mapWindowSettings = configValue;  }
            else if ( configSetting == "EXPWINDOWSETTINGS")
            {   expWindowSettings = configValue;  }
            else if ( configSetting == "STATSWINDOWSETTINGS")
            {   statsWindowSettings = configValue;  }
            else if ( configSetting == "CLOCKWINDOWSETTINGS")
            {   clockWindowSettings = configValue;  }
            else if ( configSetting == "COMBATWINDOWSETTINGS")
            {   combatWindowSettings = configValue;  }
            else if ( configSetting == "DPSWINDOWSETTINGS")
            {   dpsWindowSettings = configValue;  }
            else if ( configSetting == "QUESTWINDOWSETTINGS")
            {   questWindowSettings = configValue;  }

            else if ( configSetting == "LOGONLOGOFFSOUND" )
            {
                if ( configValue.toInt() > 0 )
                {
                    LogonLogoffSound = true;
                }
            }
            else if ( configSetting == "LOGONLOGOFFPOPUP" )
            {
                if ( configValue.toInt() > 0 )
                {
                    LogonLogoffPopup = true;
                }
            }
            else if ( configSetting == "LOGONLOGOFFTIME" )
            {
                LogonLogoffTime = configValue.toInt();
            }
            else if ( configSetting == "LOGONLOGOFFLISTON" )
            {
                if ( configValue.toInt() > 0 )
                {
                    loginNotificationListOn = true;
                }
            }
            else if ( configSetting == "LOGONLOGOFFLIST" )
            {
              //Format is names seperated by commas
                int pos = configValue.indexOf(',',0);


                while ( pos > -1 ) //We found something
                {
                    loginNotificationList.append(configValue.left(pos));
                    configValue = configValue.mid(pos+1,-1);
                    pos = configValue.indexOf(',',0);
                }

                if ( configValue.length() > 0 )
                {
                    loginNotificationList.append(configValue);
                }
            }
            else if ( configSetting == "VERSION" )
            {
                int major = 0;
                int minor = 0;
                int revision = 0;
                foundVersion = true;

                QRegExp rx("(.*)\\.(.*)\\.(.*)", Qt::CaseSensitive);

                if ( rx.indexIn(configValue,0) >= 0 )
                {
                  major=rx.cap(1).toInt();
                  minor=rx.cap(2).toInt();
                  revision=rx.cap(3).toInt();
                }


                if (( major != MajorVersion) || ( minor != MinorVersion ) || ( revision != RevisionVersion ) )
                {

                    changeSettings();
                }



            }
          }
        }
        configFile.close();
    }
    else
    {
       changeSettings();
       foundVersion = true;
    }
    if ( foundVersion == false )
    {
        changeSettings();
    }




    return 0;
}

void Config::changeSettings()
{
    ConfigDialog changeSettings(this);

    changeSettings.setWindowFlags(changeSettings.windowFlags()|Qt::WindowStaysOnTopHint);

    changeSettings.exec();
}


void Config::saveSettings()
{
    //Write it all to file
    QString configPath = QDir::currentPath() + "/settings.cfg";
    QFile configFile(configPath);

    configFile.open(QIODevice::WriteOnly|QIODevice::Truncate);


    //Prep the login List to be saved as a string
    QString loginList = "";
    for ( int i = 0; i < loginNotificationList.count();++i)
    {
        loginList += loginNotificationList[i];
        loginList += ",";
    }
    if ( loginList.count() > 0 )
    {
        loginList = loginList.left(loginList.count()-1); //Remove the last ','
    }



    QTextStream configOut(&configFile);

    configOut << "CHARACTERNAME=[" << CharacterName << "]  //Please put your name between the brackets, this is the character that is being logged into" << endl;
    configOut << "DEFAULTSAVEPATH=[" << DefaultSavePath << "] //Default path to save files Leaving it empty points it to the current directory" << endl;
    if ( appDataPathChanged == true )
    {
        configOut << "APPDATAPATH=["  << appDataPath << "] //Leave blank or remove for app default. The full path to your appData folder for Shroud of the Avatar. This folder contains folders for chatlogs, music, books, etc." << endl;
    }
    configOut << "SHOWKILLINGBLOWSKILL=[" << ((ShowKillingBlowSkillUsed) ? "1" : "0" ) << "]  //A value of 1 here will show what skill was used for each kill, a value of 0 will not." << endl;
    configOut << "SEPERATECRITICALHITS=[" << ((ShowCriticalHitsSeperate) ? "1" : "0" ) << "]  //A value of 1 here will seperate critical hits from normal hits, by placing a + after the skillname. So Bow+ would be bow critical hits." << endl;
    configOut << "STARTALWAYSONTOP=[" << ((StartOnTop) ? "1" : "0" ) << "]  //A value of 1 here will start the HUD with the always on top option turned on." << endl;
    configOut << "PARSECURRENTDAY=[" << ((ParseCurrentDay) ? "1" : "0" ) << "]		//This should generally stay to the on position, i.e. 1.  The only time you'd want this off is doing older days." << endl;
    configOut << "SALESSOUND=[" << ((SalesSound) ? "1" : "0" ) << "]		//A value of 1 here will play a sound when a new sale is made in the sales tab" << endl;
    configOut << "SALESPOPUP=[" << ((SalesPopup) ? "1" : "0" ) << "]		//A value of 1 here will show a popup when a new sale is made in the sales tab" << endl;
    configOut << "SALESTIME=[" << QString::number(SalesTime)  << "]		//How many seconds a sales notification stays up, default 10" << endl;

    configOut << "WHISPERSOUND=[" << ((WhisperSound) ? "1" : "0" ) << "]		//A value of 1 here will play a sound when a new whisper is received" << endl;
    configOut << "WHISPERPOPUP=[" << ((WhisperPopup) ? "1" : "0" ) << "]		//A value of 1 here will show a popup when you get a whisper" << endl;
    configOut << "WHISPERTIME=[" << QString::number(WhisperTime)  << "]		//How many seconds a whisper notification stays up, default 10" << endl;

    configOut << "VOLUME=[" << QString::number(Volume) << "]		//This value is the volume, on the scale of 0 (silent) to 100 (Maximum ear damage)" << endl;
    configOut << "STARTTRACKPLAYER=[" << ((StartTrackPlayer) ? "1" : "0" ) << "]		//A value of 1 here will make the map autotrack the player on startup" << endl;
    configOut << "SHOWEVENTSINXHOURS=[" << QString::number(ShowEventsHappeningInXHours) << "]		//Upcoming events will only show on the clock if they are happening in the next X horus, default 48 hours (2 days)" << endl;
    configOut << endl;
    configOut << endl;

    configOut << "FILEDATEFORMAT=[" << FileDateFormat << "] //This is pulled from system and shouldn't need changing' Date format used for log filenames " << endl;
    configOut << "LOGDATETIMEFORMAT=[" << LogDateTimeFormat << "] //This is pulled from system and shouldn't need changing' DateTime format used inside logs " << endl;
    configOut << "SYSTEMLOCALENAME=[" << SystemLocaleName << "] //This is pulled from system and shouldn't need changing." << endl;

    configOut << endl;
    configOut << endl;
    configOut << "PARSEPASTDAY=[" << ((ParsePastDay) ? "1" : "0") << "] //This should generally be left to 0, unless you are wanting to see a previous date range. " << endl;
    configOut << "PARSEPASTDAYBEGIN=[" << pastDayStart.toString("yyyy-MM-dd") << "] //The starting date, inclusive, that you want to include in the parsing. Format: yyyy-MM-dd, just like in the chat log. Leading 0s, so 2016-02-03" << endl;
    configOut << "PRASEPASTDAYEND=[" << pastDayEnd.toString("yyyy-MM-dd") << "] //The ending date, inclusive, that you want to include in the parsing. Format: yyyy-MM-dd, just like in the chat log. Leading 0s, so 2016-02-03" << endl;
    configOut << endl;
    configOut << "LOGONLOGOFFSOUND=[" << ((LogonLogoffSound) ? "1" : "0") << "] //Whether logon/logoff notifications play a sound" << endl;
    configOut << "LOGONLOGOFFPOPUP=[" << ((LogonLogoffPopup) ? "1" : "0") << "] //Whether logon/logoff notifications show a popup" << endl;
    configOut << "LOGONLOGOFFTIME=[" << QString::number(LogonLogoffTime) << "] //How long logon/logoff popups stay up, default 10" << endl;
    configOut << "LOGONLOGOFFLISTON=[" << ((loginNotificationListOn) ? "1" : "0") << "] //Whether to use the login/logoff list" << endl;
    configOut << "LOGONLOGOFFLIST=[" << loginList << "] //Current Login List" << endl;

    configOut << endl;
    configOut << endl;
    configOut << "//WARNING:  Below here is performance tuning, smaller numbers on MAX means better performance on big logs" << endl;
    configOut << "MAXDISPLAYLINES=[" << QString::number(MaxDisplay) << "]  //How many lines a given window can have before it starts clearing old ones. Note this is per window, so even though the main log may clear lines, they'll still be in chat if it hasn't hit its individual limit" << endl;
    configOut << "LINESTOCLEARWHENFULL=[" << QString::number(MaxClearLength) << "] //How many lines to clean out at a time" << endl;
    configOut << "VERSION=[" << QString::number(MajorVersion) << "." << QString::number(MinorVersion) << "." << QString::number(RevisionVersion) << "] //What version this settings file was made under" << endl;

    configOut << endl;
    configOut << endl;
    configOut << endl;
    configOut << "//INTERFACE HISTORY SETTNIGS: Probably shouldn't touch this and just let the HUD do it" << endl;
    //FINDME: ADDWINDOW
    configOut << "MAINWINDOWSETTINGS=[" << mainWindowSettings << "]" << endl;
    configOut << "MAPWINDOWSETTINGS=[" << mapWindowSettings << "]" << endl;
    configOut << "EXPWINDOWSETTINGS=[" << expWindowSettings << "]" << endl;
    configOut << "STATSWINDOWSETTINGS=[" << statsWindowSettings << "]" << endl;
    configOut << "CLOCKWINDOWSETTINGS=[" << clockWindowSettings << "]" << endl;
    configOut << "COMBATWINDOWSETTINGS=[" << combatWindowSettings << "]" << endl;
    configOut << "DPSWINDOWSETTINGS=[" << dpsWindowSettings << "]" << endl;
    configOut << "QUESTWINDOWSETTINGS=[" << questWindowSettings << "]" << endl;







    configOut.flush();
    configFile.close();
}
