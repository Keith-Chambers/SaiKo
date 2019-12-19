QT += qml quick

# Features to Implement

# (?) gradient color for seek progress bar
# Click now playing album art to return to mAudioFolder
# When you go up a directory, you reload the gridview at the same position (I.e At the same artist / album, not the beginning)
# Tile images that have low resolution
# Playlist highlight updates on song change
# Can use previous song to go all the way to the first song in a folder, regardless of which song was clicked
# Update UI to fit 4 artists for 1080p screen


CONFIG += c++17

SRC_PATH = $$PWD/src
EXTERNAL_PATH = $$PWD/external
BIN_PATH = $$PWD/bin

SOURCES +=  $$SRC_PATH/main.cpp \
            $$SRC_PATH/audiofile.cpp \
            $$SRC_PATH/audioplayer.cpp \
            $$SRC_PATH/mediaitem.cpp \
            $$SRC_PATH/mediafilesystem.cpp \
            $$SRC_PATH/soundstopevent.cpp \
            $$SRC_PATH/textparser.cpp

RESOURCES += \
    qml.qrc

QML_IMPORT_PATH =

INCLUDEPATH +=  $$EXTERNAL_PATH/taglib/ \
                $$EXTERNAL_PATH/taglib/toolkit/ \
                $$EXTERNAL_PATH/taglib/mpeg/id3v2/ \
                $$SRC_PATH/

include(deployment.pri)

HEADERS += \
    $$SRC_PATH/audiofile.h \
    $$SRC_PATH/audioplayer.h \
    $$SRC_PATH/mediaitem.h \
    $$SRC_PATH/mediafilesystem.h \
    $$SRC_PATH/soundstopevent.h \
    $$SRC_PATH/textparser.h

# Image Magick
INCLUDEPATH += $$EXTERNAL_PATH
INCLUDEPATH += $$EXTERNAL_PATH/ImageMagick-7

LIBS += -L$$EXTERNAL_PATH

# kpl
DEFINES += "KPL_USE_QT=1"
LIBS += -lkpl_static
# kpl end

LIBS += -fopenmp
LIBS += -lMagick++-7.Q16HDRI
LIBS += -lMagickWand-7.Q16HDRI
LIBS += -lMagickCore-7.Q16HDRI

DEFINES += "DMAGICKCORE_HDRI_ENABLE=1"
DEFINES += "MAGICKCORE_QUANTUM_DEPTH=16"
DEFINES += "MAGICKCORE_HDRI_ENABLE=0"
# Image Magick end

LIBS += -ltag
LIBS += -lz
LIBS += -lIrrKlang

LIBS += $$EXTERNAL_PATH/ikpMP3.so
LIBS += $$EXTERNAL_PATH/ikpFlac.so

