#-------------------------------------------------
#
# Project created by QtCreator 2018-10-26T21:45:23
#
#-------------------------------------------------

QT += core gui widgets sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = issra
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    connexion.cpp \
    main.cpp \
    mainwindow.cpp \
    planning.cpp \
    test.cpp

HEADERS += \
    connexion.h \
    mainwindow.h \
    planning.h \
    test.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
