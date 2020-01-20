QT += qml quick quickcontrols2

CONFIG += c++17
#CONFIG += static

#CONFIG  += qt warn_off release
#DEFINES += QT_NO_DEBUG_OUTPUT
#DEFINES += QT_NO_DEBUG

SRC_PATH = $$PWD/src
EXTERNAL_PATH = $$PWD/external
BIN_PATH = $$PWD/bin

SOURCES +=  $$SRC_PATH/main.cpp \
            $$SRC_PATH/audiofile.cpp \
            $$SRC_PATH/audioplayer.cpp \
            $$SRC_PATH/mediaitem.cpp \
            $$SRC_PATH/mediafilesystem.cpp \
            $$SRC_PATH/soundstopevent.cpp \
            $$SRC_PATH/textparser.cpp \
            src/audioplaylist.cpp \
            src/mediadirectory.cpp

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
    $$SRC_PATH/textparser.h \
    src/audioplaylist.h \
    src/mediadirectory.h

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

external_dependencies.path = bin/
external_dependencies.files = external/*.so

INSTALLS += external_dependencies

LIBS += -ltag
LIBS += -lz
LIBS += -lIrrKlang

LIBS += -l:ikpMP3.so
LIBS += -l:ikpFlac.so

