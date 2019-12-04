QT += qml quick

CONFIG += c++11

SOURCES += main.cpp \
    audioplayer.cpp \
    mediaitem.cpp \
    mediafilesystem.cpp \
    soundstopevent.cpp \
    textparser.cpp

RESOURCES += \
    qml.qrc

QML_IMPORT_PATH =

include(deployment.pri)

HEADERS += \
    audioplayer.h \
    mediaitem.h \
    mediafilesystem.h \
    soundstopevent.h \
    textparser.h

LIBS += -L$$PWD/libs/linux/ -lIrrKlang
LIBS += -L$$PWD/libs/linux/ikpMP3.so
LIBS += -L$$PWD/libs/linux/ikpFlac.so
