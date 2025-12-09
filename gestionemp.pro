QT       += core gui sql charts serialport printsupport multimedia multimediawidgets network websockets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    authpage.cpp \
    connection.cpp \
    employer.cpp \
    main.cpp \
    mainwindow.cpp \
    arduino.cpp \
    enfant.cpp \
    happysad.cpp \
    voicetotext.cpp \
    voicetotextdialog.cpp


HEADERS += \
    authpage.h \
    connection.h \
    employer.h \
    mainwindow.h \
    arduino.h \
    enfant.h \
    happysad.h \
    voicetotext.h \
    voicetotextdialog.h


FORMS += \
    authpage.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
