#-------------------------------------------------
#
# Project created by QtCreator 2016-06-19T13:32:15
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets multimedia

TARGET = SOTAHUD
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    location.cpp \
    config.cpp \
    configdialog.cpp \
    map.cpp \
    notification.cpp \
    addmappoint.cpp \
    explookup.cpp \
    hudwindow.cpp \
    hudmap.cpp \
    hudexp.cpp \
    hudtools.cpp \
    hudstats.cpp \
    sotacalcs.cpp \
    hudclock.cpp \
    huddecogrid.cpp \
    hudcombat.cpp \
    notificationmanager.cpp \
    huddps.cpp \
    salesexporter.cpp \
    questsplusquest.cpp \
    questspluscontroller.cpp \
    hudquest.cpp \
    hudquestsmodel.cpp \
    hudquestjournal.cpp

HEADERS  += mainwindow.h \
    location.h \
    config.h \
    configdialog.h \
    map.h \
    notification.h \
    addmappoint.h \
    explookup.h \
    hudwindow.h \
    hudmap.h \
    hudexp.h \
    hudtools.h \
    hudstats.h \
    sotacalcs.h \
    hudclock.h \
    huddecogrid.h \
    hudcombat.h \
    notificationmanager.h \
    huddps.h \
    salesexporter.h \
    questsplusquest.h \
    questspluscontroller.h \
    hudquest.h \
    hudquestsmodel.h \
    hudquestjournal.h

FORMS    += mainwindow.ui \
    configdialog.ui \
    notification.ui \
    addmappoint.ui \
    hudmap.ui \
    hudexp.ui \
    hudtools.ui \
    hudstats.ui \
    hudclock.ui \
    huddecogrid.ui \
    hudcombat.ui \
    hudquest.ui \
    hudquestjournal.ui

RESOURCES += \
    resources.qrc

DISTFILES +=
