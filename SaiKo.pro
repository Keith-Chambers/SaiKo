QT += qml quick

CONFIG += c++17

SOURCES += main.cpp \
    audiofile.cpp \
    audioplayer.cpp \
    mediaitem.cpp \
    mediafilesystem.cpp \
    soundstopevent.cpp \
    textparser.cpp

RESOURCES += \
    qml.qrc

QML_IMPORT_PATH =

INCLUDEPATH +=  $$PWD/inc/taglib/ \
                $$PWD/inc/taglib/toolkit/ \
                $$PWD/inc/taglib/mpeg/id3v2/ \
                $$PWD/inc/

include(deployment.pri)

HEADERS += \
    audiofile.h \
    audioplayer.h \
    mediaitem.h \
    mediafilesystem.h \
    soundstopevent.h \
    textparser.h

LIBS += -L$$PWD/libs/linux/

LIBS += -lIrrKlang
LIBS += ikpMP3.so
LIBS += ikpFlac.so
LIBS += -ltag
