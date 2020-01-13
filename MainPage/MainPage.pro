#-------------------------------------------------
#
# Project created by QtCreator 2018-06-21T20:43:35
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PowerRemote
TEMPLATE = app

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
        main.cpp \
        mainwindow.cpp \
    about.cpp

HEADERS += \
        mainwindow.h \
    about.h


FORMS += \
        mainwindow.ui \
    about.ui


RESOURCES += \
    image.qrc

#RC_FILE = icon.rc

RC_FILE += \
    ../icon.rc

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../ConfHostInfo/release/ -lConfHostInfo
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../ConfHostInfo/debug/ -lConfHostInfo
else:unix: LIBS += -L$$OUT_PWD/../ConfHostInfo/ -lConfHostInfo

INCLUDEPATH += $$PWD/../ConfHostInfo
DEPENDPATH += $$PWD/../ConfHostInfo

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../DisplayInfo/release/ -lDisplayInfo
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../DisplayInfo/debug/ -lDisplayInfo
else:unix: LIBS += -L$$OUT_PWD/../DisplayInfo/ -lDisplayInfo

INCLUDEPATH += $$PWD/../DisplayInfo
DEPENDPATH += $$PWD/../DisplayInfo

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../RecordInfo/release/ -lRecordInfo
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../RecordInfo/debug/ -lRecordInfo
else:unix: LIBS += -L$$OUT_PWD/../RecordInfo/ -lRecordInfo

INCLUDEPATH += $$PWD/../RecordInfo
DEPENDPATH += $$PWD/../RecordInfo

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../SqlManager/release/ -lSqlManager
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../SqlManager/debug/ -lSqlManager
else:unix: LIBS += -L$$OUT_PWD/../SqlManager/ -lSqlManager

INCLUDEPATH += $$PWD/../SqlManager
DEPENDPATH += $$PWD/../SqlManager

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../TcpManager/release/ -lTcpManager
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../TcpManager/debug/ -lTcpManager
else:unix: LIBS += -L$$OUT_PWD/../TcpManager/ -lTcpManager

INCLUDEPATH += $$PWD/../TcpManager
DEPENDPATH += $$PWD/../TcpManager


