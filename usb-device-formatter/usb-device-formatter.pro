#-------------------------------------------------
#
# Project created by QtCreator 2016-12-07T09:33:51
#
#-------------------------------------------------

QT       += core gui concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = usb-device-formatter
TEMPLATE = app

include(../partman/partman.pri)

PKGCONFIG += dtkwidget dtkbase dtkutil

CONFIG += c++11 link_pkgconfig

TRANSLATIONS += $$PWD/translations/$${TARGET}.ts \
    $$PWD/translations/$${TARGET}_zh_CN.ts

SOURCES += main.cpp \
    mainwindow.cpp \
    widgets/progressline.cpp \
    mainpage.cpp \
    warnpage.cpp \
    formatingpage.cpp \
    widgets/progressbox.cpp \
    finishpage.cpp \
    errorpage.cpp \
    app/cmdmanager.cpp

HEADERS  += \
    mainwindow.h \
    widgets/progressline.h \
    mainpage.h \
    warnpage.h \
    formatingpage.h \
    widgets/progressbox.h \
    finishpage.h \
    errorpage.h \
    app/cmdmanager.h

RESOURCES += \
    theme/theme.qrc

PREFIX = /usr
BINDIR = $$PREFIX/bin
SHAREDIR = $$PREFIX/share/$${TARGET}

target.path = $$BINDIR

translations.files = $$PWD/translations/*.qm
translations.path = $$SHAREDIR/translations

policy.path = $${PREFIX}/share/polkit-1/actions/
policy.files = pkexec/com.deepin.pkexec.usb-device-formatter.policy

pkexec.files = pkexec/usb-device-formatter-pkexec
pkexec.path = /usr/bin

DISTFILES += \
    pkexec/com.deepin.pkexec.usb-device-formatter.policy \
    pkexec/usb-device-formatter-pkexec

INSTALLS += target translations policy pkexec