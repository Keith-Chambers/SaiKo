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

#include <vector>
#include <optional>

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

class MediaFileSystem : public QObject
{
    Q_OBJECT

public:
    explicit MediaFileSystem(QObject *parent = nullptr);
    MediaFileSystem(QStringList pLibraryAbsPaths, QQmlApplicationEngine *pEngine, QObject *parent = nullptr);
    Q_INVOKABLE void upDir();
    void downDir(QString pDirName);
    void generatePlaylist(QString pSongName);
    void generateMediaItems();
    void generateMediaItemsFromRoot();
    void createSaikFiles(bool pRecheck);
    Q_INVOKABLE void invokeMediaItem(QString fileName, QString extension);

    bool isFolderContainingAudio(QDir folder);
    QStringList audioFilesInFolder(QDir folder);
    bool hasAudioContainingSubFolders(QDir folder);
    QStringList audioContainingSubFolders(QDir Folder);

    Q_INVOKABLE void playFromCurrentAudioSelection(unsigned long index);

    // TODO: Move to util class
    static QDir makeChildDir(QDir parent, QString childName);

signals:
    void playlistChanged(QStringList pPlaylist);
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
