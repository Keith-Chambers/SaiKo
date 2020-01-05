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
#include <QtQuickControls2/QQuickStyle>

#include <iostream>

#include <kpl/filesystem.h>

#include "audioplayer.h"
#include "mediafilesystem.h"

int main(int argc, char *argv[])
{
//    QQuickStyle::setStyle("Imagine");
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/resources/music.ico"));
    QQmlApplicationEngine engine;

    AudioPlayer audioplayer(&engine);

    QList<kpl::filesystem::DirectoryPath> library_roots;
    QStringList library_paths;

    QString user_config_path;

    if(false && argc == 3 && strcmp(argv[1],"-c") == 0) {
        user_config_path = argv[2];
    } else {
        user_config_path = "../configLib.txt";
    }

    // Load library from file
    QFile file("../configLib.txt");
    if(!file.open(QIODevice::ReadOnly))
    {
        std::cout << "Failed to read config" << std::endl;
        exit(EXIT_FAILURE);
        return 0;
    }else
    {
        QTextStream in(&file);
        library_paths << in.readLine();
    }

    for(const auto& lib_path : library_paths)
    {
        std::optional<kpl::filesystem::DirectoryPath> file_opt = kpl::filesystem::DirectoryPath::make(lib_path);

        if(file_opt != std::nullopt) {
            qDebug() << "Adding library root " << lib_path;
            library_roots.append(*file_opt);
        }
    }

    MediaFileSystem mFileSys(library_roots, &engine);

//    mFileSys.purgeSaikData();

    QObject::connect(&mFileSys, &MediaFileSystem::currentAudioChanged, &audioplayer, &AudioPlayer::playAudio);
    QObject::connect(&audioplayer, &AudioPlayer::audioCompleted, &mFileSys, &MediaFileSystem::nextTrack);

    engine.rootContext()->setContextProperty("AudioPlayer", QVariant::fromValue(&audioplayer));
    engine.rootContext()->setContextProperty("MFileSys", QVariant::fromValue(&mFileSys));

    engine.load(QUrl(QLatin1String("qrc:/ui/main.qml")));

    return app.exec();
}
