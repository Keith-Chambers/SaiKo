QT += qml quick

CONFIG += c++11

SOURCES += main.cpp \
    audioplayer.cpp \
    mediaitem.cpp \
    mediafilesystem.cpp \
    soundstopevent.cpp

RESOURCES += \
    qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    audioplayer.h \
    mediaitem.h \
    mediafilesystem.h \
    soundstopevent.h


LIBS += -lirrKlang
