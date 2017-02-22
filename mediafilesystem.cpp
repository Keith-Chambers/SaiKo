#include "mediafilesystem.h"

// TODO: Take out contructor
MediaFileSystem::MediaFileSystem(QObject *parent) : QObject(parent)
{
    qDebug() << "Don't use this constructor";
}

MediaFileSystem::MediaFileSystem(QStringList pLibraryAbsPaths, QQmlApplicationEngine *pEngine, QObject *parent)
    : QObject(parent)
{
    mLibraryAbsPaths = pLibraryAbsPaths;
    mEngine = pEngine;
    mDirLevelIndex = 0;
    mCurrentLibraryIndex = -1;
    mCurrentDir = Q_NULLPTR;
    mFilters << "*.flac" << "*.mp3";
}

void MediaFileSystem::upDir()
{
    qDebug() << "upDir: Lv " << mDirLevelIndex;

    switch(mDirLevelIndex)
    {
    case 0:
        return;
    case 1:
        mDirLevelIndex = 0;
        mCurrentLibraryIndex = -1;
        if(mCurrentDir)
            delete mCurrentDir;
        mCurrentDir = Q_NULLPTR;
        break;
    default:
    {
        if(mCurrentDir)
            mCurrentDir->cdUp();
        mDirLevelIndex--;
    }
    }

    generateMediaItems();
}

void MediaFileSystem::invokeMediaItem(QString pDirName)
{
    if(!mCurrentDir || !mCurrentDir->exists())
    {
        downDir(pDirName);
        return;
    }

    QFileInfoList fileInfo;
    QStringList nameFilters;
    nameFilters.append(pDirName + "*");
    fileInfo = mCurrentDir->entryInfoList(nameFilters);

    if(fileInfo.size() != 1)
    {
        qDebug() << "fileInfo size : " << fileInfo.size();
        return;
    }

    if(fileInfo.at(0).isFile())
    {
        qDebug() << "Playing from " << pDirName;
        generatePlaylist(pDirName);
    }else if(fileInfo.at(0).isDir())
    {
        downDir(pDirName);
    }

}

QString MediaFileSystem::getNameFromPath(QString pPath)
{
    int numElements = pPath.size() - 1;
    // Remove trailing / or \ if present
    if(pPath.at(numElements) == '/' || pPath.at(numElements) == '\\' )
        pPath.remove(numElements, 1);

    QChar sep('/');
    QString name = pPath.section(sep, -1, -1);

    return name;
}

void MediaFileSystem::downDir(QString pDirName)
{
    // Assert dirLevel is positive and dirName is valid
    qDebug() << "Moving down to " << pDirName;
    qDebug() << "DirLevel: " << mDirLevelIndex;

    int libNumber = 0;

    if(mDirLevelIndex == 0)
    {
        foreach(QString libPath, mLibraryAbsPaths)
        {
            QDir temp(libPath);
            if(!temp.exists())
            {
                qDebug() << libPath + " doesn't exist";
                continue;
            }

            if(temp.exists(pDirName))
            {
                qDebug() << libPath + "/" + pDirName + " == " + pDirName;
                mCurrentDir = new QDir(libPath + "/" + pDirName);
                mDirLevelIndex++;
                mCurrentLibraryIndex = libNumber;
                break;
            }
            libNumber++;
        }
    }else
    {
        if(!mCurrentDir)
        {
            qDebug() << "currentDir NULL in ::downDir()";
            return;
        }

        if(mCurrentDir->exists(pDirName))
        {
            mCurrentDir->cd(pDirName);
            mDirLevelIndex++;
            qDebug() << "cd " << pDirName << " successful";
        }
    }

    generateMediaItems();
}

// TODO: Overload function to take index etc
// I should probably make a playlist structure of something
void MediaFileSystem::generatePlaylist(QString pSongName)
{
    mPlaylist.clear();
    QStringList songNames;

    if(!mCurrentDir)
    {
        qDebug() << "Failed to generate playlist as currentDir is NULL";
        return;
    }

    songNames = mCurrentDir->entryList(mFilters, QDir::Files, QDir::Name);
    QStringList tempNames = songNames;

    for(int i = 0; i < tempNames.size(); i++)
    {
        if(tempNames.at(i) != pSongName)
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
        path = mCurrentDir->absoluteFilePath(songNames.at(i));
        mPlaylist.append(path);
    }

    emit playlistChanged(mPlaylist);
    return;
}

// *NOTE: We're assuming that the root folders only contain more folders atm
void MediaFileSystem::generateMediaItemsFromRoot()
{
    MediaItem *next;
    mQmlMediaItems.clear();
    QStringList directories;

    qDebug() << "libraryAbsPaths size : " << mLibraryAbsPaths.size();

    for(int i = 0; i < mLibraryAbsPaths.size(); i++)
    {
        QDir *rootDir = new QDir(mLibraryAbsPaths.at(i));
        if(!rootDir->exists())
        {
            qDebug() << mLibraryAbsPaths.at(i) << " doesn't exist";
            delete rootDir;
            continue;
        }

        directories = rootDir->entryList(QDir::AllDirs | QDir::NoDotAndDotDot, QDir::Name);

        for(int libIndex = 0; libIndex < directories.size(); libIndex++)
        {
            QString dirFullPath = mLibraryAbsPaths.at(i) + "/" + directories.at(libIndex);
            QDir currDir(dirFullPath);
            if(!currDir.exists())
            {
                qDebug() << directories.at(libIndex) << " doesn't exist?";
                continue;
            }

            QString dirName = getNameFromPath(dirFullPath);
            QString imagePath = dirFullPath + "/" + dirName + "_art.jpg";
            if(!currDir.exists(imagePath))
            {
                next = new MediaItem(dirFullPath, false);
            }else
            {
                qDebug() << "Found Image: " << imagePath;
                next = new MediaItem(dirFullPath, imagePath, false);
            }

            mQmlMediaItems.append(next);
        }
    }

    mEngine->rootContext()->setContextProperty("MediaList", QVariant::fromValue(mQmlMediaItems));
    qDebug() << "Root Media Items loaded successfuly";
}

QStringList MediaFileSystem::extractFolderImagePaths(QString pCurrentAbsPath)
{
    QFile saikFile(pCurrentAbsPath + "data.saik");
    QStringList result;

    if(!saikFile.exists())
    {
        qDebug() << saikFile.fileName() + " doesn't exist";
        return result;
    }

    if(!saikFile.open(QIODevice::ReadOnly))
    {
        qDebug() << "Failed to open: " + pCurrentAbsPath + "data.saik";
        return result;
    }

    QTextStream in(&saikFile);

    while(!in.atEnd())
    {
        QString line = in.readLine();
        QChar sep(':');
        QString type = line.section(sep, 0, 0);

        if(type.compare("#ART") != 0)
        {
            qDebug() << "Item extracted is not of type ART";
            continue;
        }

        line.remove(0, type.size() + 1);
        qDebug() << "Appending " + line;
        result.append(line);
    }

    return result;
}


void MediaFileSystem::generateMediaItems()
{
    qDebug() << "Generating media items..";

    QStringList directories;
    QStringList files;
    MediaItem *next;
    mQmlMediaItems.clear();

    if(!mCurrentDir || !mCurrentDir->exists())
    {
        generateMediaItemsFromRoot();
        return;
    }

    directories = mCurrentDir->entryList(QDir::AllDirs | QDir::NoDotAndDotDot, QDir::Name);

    if(directories.size() <= 0)
    {
        qDebug() << "No directories found in current folder";
        files = mCurrentDir->entryList(mFilters, QDir::Files, QDir::Name);

        for(int i = 0; i < files.size(); i++)
        {
            qDebug() << "F: " << files.at(i) << " found";
            next = new MediaItem(files.at(i), true);

            mQmlMediaItems.append((QObject *)next);
        }
    }

    for(int i = 0; i < directories.size(); i++)
    {
        qDebug() << "D: " << directories.at(i) << " found";

        //QString folderPath = mLibraryAbsPaths.at(mCurrentLibraryIndex) + directories.at(i) + "/";
        QString folderPath = mCurrentDir->absolutePath() + "/" + directories.at(i) + "/";
        QStringList imageNameList = extractFolderImagePaths(folderPath);
        QString imagePath;

        qDebug() << "FolderPath: " + folderPath;

        for(int i = 0; i < imageNameList.size(); i++)
        {
            QDir folderDir(folderPath);
            if(folderDir.exists(imageNameList.at(i)))
            {
                imagePath = folderPath + "/" + imageNameList.at(i);
                qDebug() << imagePath + " exists";
            }
        }

        if(imagePath.size() != 0)
        {
            next = new MediaItem(directories.at(i), imagePath, false);
        }else
        {
            next = new MediaItem(directories.at(i), false);
        }

        mQmlMediaItems.append((QObject *)next);
    }

    mEngine->rootContext()->setContextProperty("MediaList", QVariant::fromValue(mQmlMediaItems));

}

// TODO: This feature is currently only implemented for the first library
void MediaFileSystem::createSaikFiles(bool pRecheck)
{
    QString path;
    path = mLibraryAbsPaths.at(0);
    QStringList nameFilters;

    QDirIterator itr(path, nameFilters, QDir::AllDirs | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    nameFilters << "*.jpg";

    while(itr.hasNext())
    {
        itr.next();
        QDir currentDir(itr.filePath());

        if(currentDir.exists("data.saik"))
        {
            qDebug() << "data.saik alreay exists for : " << currentDir.absolutePath();

            if(!pRecheck)
                continue;
        }

        // create a .saik file inside the directory
        QFile saikFile(itr.filePath() + "/data.saik");

        if(!saikFile.open(QIODevice::ReadWrite))
        {
            qDebug() << "Failed to open .saik file";
            break;
        }

        if(pRecheck)
            saikFile.resize(0);

        // Load a list of image paths from within dir
        QStringList imageNames = currentDir.entryList(nameFilters, QDir::Files, QDir::Name);
        QTextStream out(&saikFile);

        foreach(QString imageName, imageNames)
        {
            out << "#ART:" << imageName << endl;
            qDebug() << "Appending " + imageName + " to saik file";
        }

        qDebug() << currentDir.absolutePath();
    }

    qDebug() << "Successfully created all saik files";
}

// TODO: This feature is currently only implemented for the first library
void MediaFileSystem::purgeSaikFiles()
{
    QString path;
    path = mLibraryAbsPaths.at(0);

    QStringList nameFilters;
    QDirIterator itr(path, nameFilters, QDir::AllDirs | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);

    while(itr.hasNext())
    {
        itr.next();
        QDir currentDir(itr.filePath());

        if(currentDir.exists("data.saik"))
        {
            QFile saikFile(itr.filePath() + "/data.saik");
            qDebug() << saikFile.fileName() + " deleted";
            saikFile.remove();
        }
    }

    qDebug() << "Successfully removed all saik files";
}


