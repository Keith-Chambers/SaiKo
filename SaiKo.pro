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
                $$PWD/inc/lib/
#                $$PWD/inc/

include(deployment.pri)

HEADERS += \
    audiofile.h \
    audioplayer.h \
    mediaitem.h \
    mediafilesystem.h \
    soundstopevent.h \
    textparser.h


# Image Magick
INCLUDEPATH += /usr/include/ImageMagick-7

LIBS += -fopenmp
LIBS += -lMagick++-7.Q16HDRI
LIBS += -lMagickWand-7.Q16HDRI
LIBS += -lMagickCore-7.Q16HDRI

DEFINES += "DMAGICKCORE_HDRI_ENABLE=1"
DEFINES += "MAGICKCORE_QUANTUM_DEPTH=16"
# Image Magick end

LIBS += -L$$PWD/libs/linux/

LIBS += -lIrrKlang
LIBS += ikpMP3.so
LIBS += ikpFlac.so
LIBS += -ltag
