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

#include "mediaitem.h"

class MediaFileSystem : public QObject
{
    Q_OBJECT
public:
    explicit MediaFileSystem(QObject *parent = 0);
    MediaFileSystem(QStringList libraryAbsPaths, QQmlApplicationEngine *engine, QObject *parent = 0);
    Q_INVOKABLE void upDir();
    void downDir(QString dirName);
    void generatePlaylist(QString songName);
    void generateMediaItems();
    void generateMediaItemsFromRoot();
    void printLibRecursive();
    Q_INVOKABLE void invokeMediaItem(QString dirName);
signals:
    void playlistChanged(QStringList playlist);
public slots:

private:
    // Functions
    QString getNameFromPath(QString path);

    // Variables
    QDir *currentDir;
    int dirLevel;
    int currLibrary;
    QStringList playlist;
    QStringList libraryAbsPaths;
    QString currRelativePath;
    QStringList filters;
    QQmlApplicationEngine *engine;
    QObjectList qmlMediaItems;
};

#endif // MEDIAFILESYSTEM_H
