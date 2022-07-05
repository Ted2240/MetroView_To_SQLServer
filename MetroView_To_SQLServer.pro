QT += core gui
QT += sql
QT += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Logger.cpp \
    SQL.cpp \
    about.cpp \
    adddialog.cpp \
    device.cpp \
    function.cpp \
    loading.cpp \
    loginwindow.cpp \
    logset.cpp \
    main.cpp \
    mainwindow.cpp \
    orcle.cpp

HEADERS += \
    Logger.h \
    LoggerTemplate.h \
    SQL.h \
    about.h \
    adddialog.h \
    device.h \
    function.h \
    loading.h \
    loginwindow.h \
    logset.h \
    mainwindow.h \
    orcle.h

FORMS += \
    about.ui \
    adddialog.ui \
    loading.ui \
    loginwindow.ui \
    logset.ui \
    mainwindow.ui \
    orcle.ui

TRANSLATIONS += \
    MetroView_To_SQLServer_zh_CN.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RC_ICONS = favicon.ico


