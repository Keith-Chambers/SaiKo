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

#include "mediaitem.h"
#include "audiofile.h"

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

    // TODO: Move to util class
    static QDir makeChildDir(QDir parent, QString childName);

signals:
    void playlistChanged(QStringList pPlaylist);
public slots:
    void purgeSaikFiles();
private:
    // Functions
    QString getNameFromPath(QString pPath);
    QStringList extractFolderImagePaths(QString pCurrentAbsPath);

    void loadAudioFromFolder(QDir folder);

    // Variables
    QDir *mCurrentDir;
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
