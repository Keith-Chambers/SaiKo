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

#include <kpl/filesystem.h>

#include "audioplayer.h"
#include "mediafilesystem.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/resources/music.ico"));
    QQmlApplicationEngine engine;

    AudioPlayer audioplayer(&engine);

    QList<kpl::filesystem::DirectoryPath> library_roots;
    QStringList library_paths;

    // Load library from file..
    QFile file("../configLib.txt");
    if(!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Failed to open file";
        library_paths << "./media";
    }else
    {
        QTextStream in(&file);
        library_paths << in.readLine();
//        in >> libPath;
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
//    mFileSys.purgeSaikFiles();

//    mFileSys.createSaikIndex(false);

    // Connect signals to slots

    // When current Audio is changed, it should be sent to audio player
    QObject::connect(&mFileSys, &MediaFileSystem::currentAudioChanged, &audioplayer, &AudioPlayer::playAudio);
    QObject::connect(&audioplayer, &AudioPlayer::audioCompleted, &mFileSys, &MediaFileSystem::nextTrack);

    engine.rootContext()->setContextProperty("AudioPlayer", QVariant::fromValue(&audioplayer));
    engine.rootContext()->setContextProperty("MFileSys", QVariant::fromValue(&mFileSys));

    engine.load(QUrl(QLatin1String("qrc:/ui/main.qml")));

    return app.exec();
}
