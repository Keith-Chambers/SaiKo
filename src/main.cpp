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

#include <rapidjson/document.h>

#include <kpl/filesystem.h>
#include <kpl/kfile.h>

#include "audioplayer.h"
#include "mediafilesystem.h"

struct Options
{
    QStringList library_paths;
    bool enable_tooltips;
    bool enabled_edit_mode;
};

Options loadOptions(std::string config_path)
{
    Options options;
    std::string json = kpl::loadTextFile(config_path);
    rapidjson::Document document;

    document.Parse(json.c_str());

    assert(document.IsObject());

    for(u32 i = 0; i < document["libraryPaths"].Size(); i++) {
        options.library_paths += document["libraryPaths"][i].GetString();
    }

    return options;
}

int main(int argc, char *argv[])
{
//    QQuickStyle::setStyle("Imagine");
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    std::cout << "Here" << std::endl;

    QGuiApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/resources/saiko_logo.ico"));
    QQmlApplicationEngine engine;

    AudioPlayer audioplayer(&engine);

    QList<kpl::filesystem::DirectoryPath> library_roots;
    QStringList library_paths;

    Options options = loadOptions("./settings.json");

    for(const auto& lib_path : options.library_paths) {
        library_roots += *kfs::DirectoryPath::make(lib_path);
    }

    MediaFileSystem mFileSys(library_roots, &engine);

//    mFileSys.generateSaikoMetaDataRecursive(kfs::DirectoryPath::make(library_roots.front().absolutePath()).value(), false);

//    mFileSys.purgeSaikData();

    QObject::connect(&mFileSys, &MediaFileSystem::currentAudioChanged, &audioplayer, &AudioPlayer::playAudio);
    QObject::connect(&audioplayer, &AudioPlayer::audioCompleted, &mFileSys, &MediaFileSystem::nextTrack);

    engine.rootContext()->setContextProperty("AudioPlayer", QVariant::fromValue(&audioplayer));
    engine.rootContext()->setContextProperty("MFileSys", QVariant::fromValue(&mFileSys));

    std::cout << "Pre load" << std::endl;

    engine.load(QUrl(QLatin1String("qrc:///ui/main.qml")));

    std::cout << "Post load" << std::endl;

    return app.exec();
}
