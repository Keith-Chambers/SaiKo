#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QDebug>
#include <QQmlContext>
#include <QDir>
#include <QStringList>
#include <QList>
#include <QObject>
#include <QIcon>
#include <QFile>
#include <QTextStream>

#include "audioplayer.h"
#include "mediaitem.h"
#include "mediafilesystem.h"
#include "soundstopevent.h"

#include "inc/irrKlang.h"
#include "inc/conio.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/resources/music.ico"));
    QQmlApplicationEngine engine;

    AudioPlayer audioplayer(&engine);

    QStringList libPaths;
    QString libPath;

    // Load library from file..
    QFile file("./configLib.txt");
    if(!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Failed to open file";
        libPath = "./media";
    }else
    {
        QTextStream in(&file);
        libPath = in.readLine();
//        in >> libPath;
    }

    libPaths.append(libPath);

    qDebug() << libPath;

    MediaFileSystem mFileSys(libPaths, &engine);
    mFileSys.generateMediaItems();

//    mFileSys.createSaikIndex(false);
//    mFileSys.purgeSaikData();

    // Connect signals to slots
    QObject::connect(&mFileSys, &MediaFileSystem::playlistChanged, &audioplayer, &AudioPlayer::setPlaylist);
//    QObject::connect(&audioplayer, &AudioPlayer::songChanged, &currentAudio, &AudioPlayer::setPlaylist);

    engine.rootContext()->setContextProperty("AudioPlayer", QVariant::fromValue(&audioplayer));
    engine.rootContext()->setContextProperty("MFileSys", QVariant::fromValue(&mFileSys));

    engine.load(QUrl(QLatin1String("qrc:/ui/main.qml")));


    return app.exec();
}
