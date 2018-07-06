#-------------------------------------------------
#
# Project created by QtCreator 2018-06-22T13:22:12
#
#-------------------------------------------------

QT       += widgets sql

QT       -= gui

TARGET = ConfHostInfo
TEMPLATE = lib

DEFINES += CONFHOSTINFO_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        confhostinfo.cpp \
    confhost.cpp

HEADERS += \
        confhostinfo.h \
        confhostinfo_global.h \ 
    confhost.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS += \
    confhost.ui



win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../SqlManager/release/ -lSqlManager
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../SqlManager/debug/ -lSqlManager
else:unix: LIBS += -L$$OUT_PWD/../SqlManager/ -lSqlManager

INCLUDEPATH += $$PWD/../SqlManager
DEPENDPATH += $$PWD/../SqlManager
