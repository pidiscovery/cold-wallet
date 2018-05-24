#-------------------------------------------------
#
# Project created by QtCreator 2018-03-15T13:39:46
#
#-------------------------------------------------

QT       += core gui websockets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PiColdWallet
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

ICON = res/icons/logo.icns
RC_ICONS = res/icons/logo.ico

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += "/usr/local/include"
INCLUDEPATH += "../vendor/pi/include"

LIBS += \
    -L"/usr/local/lib" \
    -L"../vendor/pi/lib" \
    -L"../vendor/pi/lib/cryptonomex" \
    -lssl -lcrypto \
    -lboost_thread -lboost_coroutine -lboost_filesystem \
    -lboost_date_time -lboost_system -lboost_chrono -lboost_context \
    -lgraphene_chain \
    -lgraphene_utilities \
    -lgraphene_db \
    -lfc \
    -lsecp256k1

SOURCES += \
    main.cpp \
    pihomewidget.cpp \
    piwebsocketclient.cpp \
    pichainutility.cpp \
    piwallet.cpp \
    witnessvotedialog.cpp

HEADERS += \
    pihomewidget.h \
    piwebsocketclient.h \
    pichainutility.h \
    piwallet.h \
    witnessvotedialog.h

FORMS += \
    pihomewidget.ui \
    witnessvotedialog.ui

RESOURCES += \
    resource.qrc
