#include "mediafilesystem.h"

// TODO: Take out contructor
MediaFileSystem::MediaFileSystem(QObject *parent) : QObject(parent)
{
    qDebug() << "Don't use this constructor";
}

MediaFileSystem::MediaFileSystem(QStringList libraryAbsPaths, QQmlApplicationEngine *engine, QObject *parent)
    : QObject(parent)
{
    this->libraryAbsPaths = libraryAbsPaths;
    this->engine = engine;
    dirLevel = 0;
    currLibrary = -1;
    currentDir = Q_NULLPTR;
    filters << "*.flac" << "*.mp3";
}

void MediaFileSystem::upDir()
{
    qDebug() << "upDir: Lv " << dirLevel;

    switch(dirLevel)
    {
    case 0:
        return;
    case 1:
        dirLevel = 0;
        currLibrary = -1;
        if(currentDir)
            delete currentDir;
        currentDir = Q_NULLPTR;
        break;
    default:
    {
        if(currentDir)
            currentDir->cdUp();
        dirLevel--;
    }
    }

    generateMediaItems();
}

void MediaFileSystem::invokeMediaItem(QString dirName)
{
    if(!currentDir || !currentDir->exists())
    {
        downDir(dirName);
        return;
    }

    qDebug() << "Dir exists";

    QFileInfoList fileInfo;
    QStringList nameFilters;
    nameFilters.append(dirName + "*");
    fileInfo = currentDir->entryInfoList(nameFilters);

    if(fileInfo.size() != 1)
    {
        qDebug() << "fileInfo size : " << fileInfo.size();
        return;
    }

    if(fileInfo.at(0).isFile())
    {
        qDebug() << "Playing from " << dirName;
        generatePlaylist(dirName);
    }else if(fileInfo.at(0).isDir())
    {
        downDir(dirName);
    }

}

QString MediaFileSystem::getNameFromPath(QString path)
{
    int numElements = path.size() - 1;
    // Remove trailing / or \ if present
    if(path.at(numElements) == '/' || path.at(numElements) == '\\' )
        path.remove(numElements, 1);

    QChar sep('/');
    QString name = path.section(sep, -1, -1);

    return name;
}

void MediaFileSystem::downDir(QString dirName)
{
    // Assert dirLevel is positive and dirName is valid
    qDebug() << "Moving down to " << dirName;
    qDebug() << "DirLevel: " << dirLevel;

    int libNumber = 0;

    if(dirLevel == 0)
    {
        foreach(QString libPath, libraryAbsPaths)
        {
            QDir temp(libPath);
            if(!temp.exists())
            {
                qDebug() << libPath + " doesn't exist";
                continue;
            }

            if(temp.exists(dirName))
            {
                qDebug() << libPath + "/" + dirName + " == " + dirName;
                currentDir = new QDir(libPath + "/" + dirName);
                dirLevel++;
                currLibrary = libNumber;
                break;
            }
            libNumber++;
        }
    }else
    {
        if(!currentDir)
        {
            qDebug() << "currentDir NULL in ::downDir()";
            return;
        }

        if(currentDir->exists(dirName))
        {
            currentDir->cd(dirName);
            dirLevel++;
            qDebug() << "cd " << dirName << " successful";
        }
    }

    generateMediaItems();
}

// TODO: Overload function to take index etc
// I should probably make a playlist structure of something
void MediaFileSystem::generatePlaylist(QString songName)
{
    playlist.clear();
    QStringList songNames;

    if(!currentDir)
    {
        qDebug() << "Failed to generate playlist as currentDir is NULL";
        return;
    }

    songNames = currentDir->entryList(filters, QDir::Files, QDir::Name);
    QStringList tempNames = songNames;

    for(int i = 0; i < tempNames.size(); i++)
    {
        if(tempNames.at(i) != songName)
        {
            //qDebug() << "Skipping " << tempNames.at(i);
            songNames.pop_front();
        }else
        {
            qDebug() << "Found song at index : " << i + 1;
            break;
        }
    }

    qDebug() << "Generating playlist absolute paths";
    for(int i = 0; i < songNames.size(); i++)
    {
        QString path;
        path = currentDir->absoluteFilePath(songNames.at(i));
        playlist.append(path);
        //qDebug() << playlist.at(i);
    }

    emit playlistChanged(playlist);
    return;
}

// *NOTE: We're assuming that the root folders only contain more folders atm
void MediaFileSystem::generateMediaItemsFromRoot()
{
    MediaItem *next;
    qmlMediaItems.clear();
    QStringList directories;
    qDebug() << "libraryAbsPaths size : " << libraryAbsPaths.size();

    for(int i = 0; i < libraryAbsPaths.size(); i++)
    {
        QDir *rootDir = new QDir(libraryAbsPaths.at(i));
        if(!rootDir->exists())
        {
            qDebug() << libraryAbsPaths.at(i) << " doesn't exist";
            delete rootDir;
            continue;
        }

        directories = rootDir->entryList(QDir::AllDirs | QDir::NoDotAndDotDot, QDir::Name);

        for(int libIndex = 0; libIndex < directories.size(); libIndex++)
        {
            QString dirFullPath = libraryAbsPaths.at(i) + "/" + directories.at(libIndex);
            QDir currDir(dirFullPath);
            if(!currDir.exists())
            {
                qDebug() << directories.at(libIndex) << " doesn't exist?";
                continue;
            }

            //qDebug() << "Full Path: " << dirFullPath;

            QString dirName = getNameFromPath(dirFullPath);
            QString imagePath = dirFullPath + "/" + dirName + "_art.jpg";
            if(!currDir.exists(imagePath))
            {
                //qDebug() << imagePath << " doesn't exist";
                next = new MediaItem(dirFullPath, false);
            }else
            {
                qDebug() << "Found Image: " << imagePath;
                next = new MediaItem(dirFullPath, imagePath, false);
            }

            qmlMediaItems.append(next);
        }
    }

    qDebug() << "Loading to context";
    engine->rootContext()->setContextProperty("MediaList", QVariant::fromValue(qmlMediaItems));
    qDebug() << "Complete";
}


void MediaFileSystem::generateMediaItems()
{
    qDebug() << "Generating media items..";

    QStringList directories;
    QStringList files;
    MediaItem *next;
    qmlMediaItems.clear();

    if(!currentDir || !currentDir->exists())
    {
        qDebug() << "At root directory";
        generateMediaItemsFromRoot();
        return;
    }

    qDebug() << "CurrentDir exists";

    directories = currentDir->entryList(QDir::AllDirs | QDir::NoDotAndDotDot, QDir::Name);

    qDebug() << "directories fetched";

    if(directories.size() <= 0)
    {
        qDebug() << "No directories found in current folder";
        files = currentDir->entryList(filters, QDir::Files, QDir::Name);

        for(int i = 0; i < files.size(); i++)
        {
            qDebug() << "F: " << files.at(i) << " found";
            next = new MediaItem(files.at(i), true);

            qmlMediaItems.append((QObject *)next);
        }

        //engine->rootContext()->setContextProperty("MediaList", QVariant::fromValue(qmlMediaItems));
        //return;
    }

    for(int i = 0; i < directories.size(); i++)
    {
        qDebug() << "D: " << directories.at(i) << " found";

        QString imageDirString = libraryAbsPaths.at(currLibrary) + "/" + currRelativePath +
                "/" + directories.at(i) + "/";
        QDir imageDir(imageDirString);
        QString imageName = directories.at(i) + "_art.jpg";

        qDebug() << "Searching for Image at : " << imageDirString + imageName;

        if(imageDir.exists(imageName))
        {
            qDebug() << "Image found for directory";
            QString imagePath = imageDirString + imageName;
            next = new MediaItem(directories.at(i), imagePath, false);
        }else
        {
            next = new MediaItem(directories.at(i), false);
        }

        qmlMediaItems.append((QObject *)next);
    }

    engine->rootContext()->setContextProperty("MediaList", QVariant::fromValue(qmlMediaItems));

}

void MediaFileSystem::printLibRecursive()
{
    QFile file("./TEST.txt");

    if(!file.open(QIODevice::ReadWrite))
    {
        qDebug() << "Failed to open TEST.txt";
        return;
    }

    qDebug() << "Successfully created";
}



