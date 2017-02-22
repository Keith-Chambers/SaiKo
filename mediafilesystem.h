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

#include "mediaitem.h"

class MediaFileSystem : public QObject
{
    Q_OBJECT
public:
    explicit MediaFileSystem(QObject *parent = 0);
    MediaFileSystem(QStringList pLibraryAbsPaths, QQmlApplicationEngine *pEngine, QObject *parent = 0);
    Q_INVOKABLE void upDir();
    void downDir(QString pDirName);
    void generatePlaylist(QString pSongName);
    void generateMediaItems();
    void generateMediaItemsFromRoot();
    void createSaikFiles(bool pRecheck);
    Q_INVOKABLE void invokeMediaItem(QString pDirName);
signals:
    void playlistChanged(QStringList pPlaylist);
public slots:
    void purgeSaikFiles();
private:
    // Functions
    QString getNameFromPath(QString pPath);
    QStringList extractFolderImagePaths(QString pCurrentAbsPath);

    // Variables
    QDir *mCurrentDir;
    int mDirLevelIndex;
    int mCurrentLibraryIndex;
    QStringList mPlaylist;
    QStringList mLibraryAbsPaths;
    QString mCurrentRelativePath;
    QStringList mFilters;
    QQmlApplicationEngine *mEngine;
    QObjectList mQmlMediaItems;
};

#endif // MEDIAFILESYSTEM_H
