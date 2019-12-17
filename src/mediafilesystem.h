#ifndef MEDIAFILESYSTEM_H
#define MEDIAFILESYSTEM_H

#include <QObject>
#include <QStringList>
#include <QDir>
#include <QDebug>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QVariant>
#include <QFile>
#include <QFileInfoList>
#include <QDirIterator>
#include <QQmlProperty>

#include <vector>
#include <optional>

#include <Magick++.h>

#include <fileref.h>
#include <tag.h>
#include <mpeg/id3v2/id3v2tag.h>
#include <mpeg/id3v2/id3v2header.h>
#include <mpeg/id3v2/id3v2frame.h>
#include <mpeg/mpegfile.h>
#include <mpeg/id3v2/frames/attachedpictureframe.h>

#include <tiostream.h>
#include <tfile.h>
#include <tfilestream.h>

#include "mediaitem.h"
#include "audiofile.h"
#include "textparser.h"

// TODO: move
struct Resolution
{
    uint32_t width;
    uint32_t height;
};

class FileIdentifier
{
public:
    FileIdentifier(QString absolutePath);
    FileIdentifier(QDir dir, QString fileName);
    FileIdentifier(QString dirPath, QString fileName);

    FileIdentifier(const FileIdentifier& copyFrom);
    FileIdentifier(const FileIdentifier&& moveFrom);

    QString fileAbsolutePath();
    QString dirAbsolutePath();
    QString fileName();
    QString fileExtension();
    QString fileNameWithExtension();

private:
    QString name;
    QString path;
    QString extension;
};

typedef QStringList AbsoluteFilePathList;

class MediaFileSystem : public QObject
{
    Q_OBJECT

public:

    Q_PROPERTY(QString currentFolderName READ getCurrentFolderName NOTIFY directoryChanged)
    Q_PROPERTY(QString currentAudioFolderName READ getCurrentAudioFolderName NOTIFY currentAudioFolderChanged)

    // NEW API BEGIN

//    Q_PROPERTY(QString libraryViewDirPath READ getLibraryViewDirPath NOTIFY libraryViewDirChanged)
//    Q_PROPERTY(QString libraryViewDirName READ getLibraryViewDirName NOTIFY libraryViewDirChanged)

//    Q_PROPERTY(QString audioViewDirPath READ getAudioViewDirPath NOTIFY audioViewDirChanged)
//    Q_PROPERTY(QString audioViewDirName READ getAudioViewDirName NOTIFY audioViewDirChanged)

//    Q_PROPERTY(QQmlProperty<AudioFile> musicPlaylist READ getMusicPlaylist NOTIFY musicPlaylistChanged)
//    Q_PROPERTY(int currentPlaylistIndex READ getCurrentPlaylistIndex NOTIFY currentPlaylistIndex)

//public:
//    Q_INVOKABLE void cdUp();
//    Q_INVOKABLE void invokeFolder(QString folderName);

//private:
//    void cdDown(QString dirName);
//    void appendTrackToPlaylist(QString fileName);
//    void makeCurrentFolderPlaylist();
//    void generateSaikoMetaData(QDir dir, bool recheck);
//    void loadCurrentDirectoryQMLData();

//    static bool dirContainsAudio(QDir dir);
//    static QStringList audioInDir(QDir dir);

//    static bool dirContainsAudioRecursive(QDir dir);
//    static QStringList audioInDirRecursive(QDir dir);

    // NEW API END
public:

    explicit MediaFileSystem(QObject *parent = nullptr);
    MediaFileSystem(QStringList pLibraryAbsPaths, QQmlApplicationEngine *pEngine, QObject *parent = nullptr);
    Q_INVOKABLE void upDir();
    void downDir(QString pDirName);

    void generatePlaylist(QString pSongName);
    void generateMediaItems();
    void generateMediaItemsFromRoot();
    void createSaikFiles(bool pRecheck);
    void createSaikIndex(bool pRecheck);

    Q_INVOKABLE void invokeMediaItem(QString fileName, QString extension);

    QString getCurrentAudioFolderName() {
        return (mAudioFolder != std::nullopt) ? mAudioFolder.value().dirName() : "";
    }

    bool isFolderContainingAudio(QDir folder);
    QStringList audioFilesInFolder(QDir folder);
    bool hasAudioContainingSubFolders(QDir folder);
    QStringList audioContainingSubFolders(QDir Folder);

    QString getCurrentFolderName() {
        return (mCurrentDir != nullptr) ? mCurrentDir->dirName() : "Your Music Library";
    }

    void purgeSaikData();

    Q_INVOKABLE void playFromCurrentAudioSelection(unsigned long index);

    // TODO: Move to util class
    static QDir makeChildDir(QDir parent, QString childName);

signals:
    void playlistChanged(QStringList pPlaylist);
    void directoryChanged();
    void currentAudioFolderChanged();
public slots:
    void purgeSaikFiles();
private:

    static QString getAudioTitle(QFile audioFile);
    static QString getAudioArtist(QFile audioFile);
    static QString getAudioAlbum(QFile audioFile);

//    static void getAudioArt(QFile audioFile);

    static QString loadAlbumArtToFileIfExists(QString filePath, QString successPath, QString failurePath);

    // Functions
    QString getNameFromPath(QString pPath);
    QStringList extractFolderImagePaths(QString pCurrentAbsPath);

    static uint16_t availableImagesInSubFolders(QDir directory);
    static QStringList getBestImagesPaths(QDir directory, uint16_t numImages, Resolution res);
    static QString bestResolution(QString first, QString second, Resolution targetRes);
    static QString bestImageOf(QDir directory, QStringList images, Resolution res);
    static std::optional<Magick::Image> createTiledImage(QStringList sourceImagesPaths, Resolution res);

    void loadAudioFromFolder(QDir folder);

    // Variables
    QDir *mCurrentDir;

    QString mCurrentAlbumImagePath;

    std::optional<QDir> mAudioFolder;

    int mDirLevelIndex;
    int mCurrentLibraryIndex;
    QStringList mPlaylist;
    QStringList mLibraryAbsPaths;
    QString mCurrentRelativePath;
    QStringList mFilters;
    QQmlApplicationEngine *mEngine;

    QObjectList mQmlCurrentFolderAudioFiles;
    std::vector<AudioFile> mCurrentFolderAudioFiles;

    QObjectList mQmlMediaItems;
};

#endif // MEDIAFILESYSTEM_H
