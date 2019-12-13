#include "mediafilesystem.h"

// TODO: Take out contructor
MediaFileSystem::MediaFileSystem(QObject *parent) : QObject(parent)
{
    qDebug() << "Don't use this constructor";

    mEngine->rootContext()->setContextProperty("CurrentFolderAudioList", QVariant::fromValue(mQmlCurrentFolderAudioFiles));
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

    mEngine->rootContext()->setContextProperty("CurrentFolderAudioList", QVariant::fromValue(mQmlCurrentFolderAudioFiles));
}

QString MediaFileSystem::getAudioTitle(QFile audioFile)
{
    TagLib::FileRef f(audioFile.fileName().toStdString().c_str());

    if(f.isNull()) {
        qDebug() << "Failed to get title for " << audioFile;
        return "";
    }

    TagLib::Tag *tag = f.tag();

    return QString::fromStdString(std::string(tag->title().to8Bit()));
}

QString MediaFileSystem::getAudioArtist(QFile audioFile)
{
    TagLib::FileRef f(audioFile.fileName().toStdString().c_str());
    TagLib::Tag *tag = f.tag();

    return QString::fromStdString(std::string(tag->artist().to8Bit()));
}

QString MediaFileSystem::getAudioAlbum(QFile audioFile)
{
    TagLib::FileRef f(audioFile.fileName().toStdString().c_str());
    TagLib::Tag *tag = f.tag();

    return QString::fromStdString(std::string(tag->album().to8Bit()));
}

QString MediaFileSystem::loadAlbumArtToFileIfExists(QString filePath, QString successPath, QString failurePath)
{
    static const char *IdPicture = "APIC" ;

    std::string path = filePath.toStdString();

    TagLib::MPEG::File mpegFile(path.c_str());
    TagLib::ID3v2::Tag *id3v2tag = mpegFile.ID3v2Tag();
    TagLib::ID3v2::FrameList Frame ;
    TagLib::ID3v2::AttachedPictureFrame *PicFrame ;
    void *SrcImage;
    unsigned long Size;

    if (id3v2tag)
    {
        Frame = id3v2tag->frameListMap()[IdPicture] ;

        if (!Frame.isEmpty() )
        {
            // find cover art
            for(TagLib::ID3v2::FrameList::ConstIterator it = Frame.begin(); it != Frame.end(); ++it)
            {
                PicFrame = (TagLib::ID3v2::AttachedPictureFrame *)(*it);

                if(PicFrame->type() == TagLib::ID3v2::AttachedPictureFrame::FrontCover)
                {
                    // extract image (in it's compressed form)
                    Size = PicFrame->picture().size();
                    SrcImage = malloc ( Size );

                    if(SrcImage)
                    {
                        FILE *jpegFile = fopen(successPath.toStdString().c_str(), "wb");

                        memcpy (SrcImage, PicFrame->picture().data(), Size) ;
                        fwrite(SrcImage, Size, 1, jpegFile);
                        fclose(jpegFile);

                        free(SrcImage);

                        return successPath;
                    }
                }
            }
        }
    }

    qDebug() << "Failed to write album art";

    return failurePath;
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

void MediaFileSystem::loadAudioFromFolder(QDir folder)
{
    qDebug() << "Loading audio files from -> " << folder.absolutePath();

    QStringList audioFileNames = audioFilesInFolder(folder);

    mQmlCurrentFolderAudioFiles.clear();
    mCurrentFolderAudioFiles.clear();

    mQmlCurrentFolderAudioFiles.reserve(audioFileNames.size());
    mCurrentFolderAudioFiles.reserve(static_cast<unsigned long>(audioFileNames.size()));

    QString albumArtLocation = "/home/keith/current_album_art.jpg";

    for(auto& audioFileName : audioFileNames) {
        qDebug() << "Adding: " << audioFileName;

//        QString artist = MediaFileSystem::getAudioArtist(audioFileName);
        QString title = MediaFileSystem::getAudioTitle(makeChildDir(folder, audioFileName).absolutePath());
        if(title == "") {
            title = audioFileName;
        }

//        QString album = MediaFileSystem::getAudioAlbum(audioFileName);

        AudioFile audioFile(title, "artist");
        audioFile.setFileName(audioFileName);

        assert(audioFile.getFileName() != "" && audioFile.getFileName() == audioFileName);

//        audioFile.setArtPath( loadAlbumArtToFileIfExists(folder.absolutePath() + "/" + audioFileName, albumArtLocation, frontImageForFolder(mAudioFolder.value())) );

        qDebug() << "Art path -> " << audioFile.getArtPath();

        mCurrentFolderAudioFiles.push_back(std::move(audioFile));
        mQmlCurrentFolderAudioFiles.append(&mCurrentFolderAudioFiles.back());
    }

    mEngine->rootContext()->setContextProperty("CurrentFolderAudioList", QVariant::fromValue(mQmlCurrentFolderAudioFiles));
}

void MediaFileSystem::invokeMediaItem(QString pFileName, QString pExtension)
{
    qDebug() << "Invoking media item";

    if(!mCurrentDir || !mCurrentDir->exists())
    {
        downDir(pFileName);
        return;
    }

    if(pExtension != "") {
        generatePlaylist(pFileName + "." + pExtension);
    } else {

        if(hasAudioContainingSubFolders(makeChildDir(*mCurrentDir, pFileName))) {
            downDir(pFileName);
        }

        if(isFolderContainingAudio(makeChildDir(*mCurrentDir, pFileName))) {
            qDebug() << "Loading audio files";

            mAudioFolder = makeChildDir(*mCurrentDir, pFileName);
            loadAudioFromFolder(makeChildDir(*mCurrentDir, pFileName));
        }
    }
}

void MediaFileSystem::playFromCurrentAudioSelection(unsigned long index)
{
    if(mAudioFolder == std::nullopt) {
        qDebug() << "No audio folder set";
        return;
    }

    mPlaylist.clear();

    for(unsigned long i = index; i < mCurrentFolderAudioFiles.size(); i++) {
        assert(mCurrentFolderAudioFiles[i].getFileName() != "");
        mPlaylist.append(mAudioFolder.value().absolutePath() + "/" + mCurrentFolderAudioFiles[i].getFileName());
    }

    emit playlistChanged(mPlaylist);
}

QString MediaFileSystem::getNameFromPath(QString pPath)
{
    int numElements = pPath.size() - 1;
    // TODO: Remove trailing / or \ if present
    if(pPath.at(numElements) == '/' || pPath.at(numElements) == '\\' )
        pPath.remove(numElements, 1);

    QChar sep('/');
    QString name = pPath.section(sep, -1, -1);

    return name;
}

void MediaFileSystem::downDir(QString pDirName)
{
    // Assert dirLevel is positive and dirName is valid
//    qDebug() << "Moving down to " << pDirName;
//    qDebug() << "DirLevel: " << mDirLevelIndex;

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
//            qDebug() << "cd " << pDirName << " successful";
        }
    }

    generateMediaItems();
}

// TODO: Overload function to take index etc
// I should probably make a playlist structure of something
// TODO: Add every song in the folder to the playlist and just jump to the song the user clicked.
// This will allow the user to use previous song to get all the way to the beginning of the song list in the folder
void MediaFileSystem::generatePlaylist(QString pSongName)
{
    mPlaylist.clear();
    QStringList songNames;

//    qDebug() << "Song to play -> " << pSongName;

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
            qDebug() << "Skipping " << tempNames.at(i);
            songNames.pop_front();
        }else
        {
            qDebug() << "Found song at index : " << i + 1;
            break;
        }
    }

//    qDebug() << "Generating playlist absolute paths";
//    qDebug() << "Songs:";
    for(int i = 0; i < songNames.size(); i++)
    {
//        qDebug() << songNames[i];
        QString path;
        path = mCurrentDir->absoluteFilePath(songNames.at(i));
        mPlaylist.append(path);
    }

    emit playlistChanged(mPlaylist);
    return;
}

uint16_t MediaFileSystem::availbleImagesInSubFolders(QDir directory)
{
    QStringList acceptedImageExtensions;
    acceptedImageExtensions << "*.jpg";
//    acceptedImageExtensions << "*.png";

    uint16_t result = 0;

    QDirIterator itr(directory.absolutePath(), QDir::Files | QDir::NoDotAndDotDot, QDirIterator::NoIteratorFlags);

    while(itr.hasNext())
    {
        itr.next();
        QDir currentDir(itr.filePath());

        QStringList imagesInFolder = currentDir.entryList(acceptedImageExtensions, QDir::Files, QDir::Name);
        result += imagesInFolder.size();

//        for(const QString& imageInFolder : imagesInFolder)
//        {

//        }
    }

    return result;
}

bool MediaFileSystem::hasAudioContainingSubFolders(QDir folder)
{
    assert(folder.exists() && "hasAudioContainingSubFolders passed folder that doesn't exist");
    QStringList subFolders = folder.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);

    for(const auto& subFolder : subFolders) {
        qDebug() << "Checking ->" << subFolder;
        QDir currentFolder = makeChildDir(folder, subFolder);
        if(isFolderContainingAudio(currentFolder) || hasAudioContainingSubFolders(currentFolder)) {
            qDebug() << "Has audio!";
            return true;
        }
    }

    qDebug() << folder.absolutePath() << " does not have audio";

    return false;
}

QDir MediaFileSystem::makeChildDir(QDir parent, QString childName)
{
    return QDir(parent.absolutePath() + "/" + childName);
}

QStringList MediaFileSystem::audioContainingSubFolders(QDir folder)
{
    QStringList subFolders = folder.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
    QStringList audioContainingSubFolders;

    for(const auto& subFolder : subFolders) {
        if(hasAudioContainingSubFolders(makeChildDir(folder,subFolder))) {
            audioContainingSubFolders.append(subFolder);
        }
    }

    return audioContainingSubFolders;
}

QStringList MediaFileSystem::audioFilesInFolder(QDir folder)
{
    QStringList audioExtensionFilters;
    audioExtensionFilters << "*.mp3";
    audioExtensionFilters << "*.flac";
    audioExtensionFilters << "*.FLAC";
    audioExtensionFilters << "*.wav";
    audioExtensionFilters << "*.WAV";
    audioExtensionFilters << "*.ogg";
    audioExtensionFilters << "*.OGG";

    QStringList audioFiles = folder.entryList(audioExtensionFilters, QDir::Files, QDir::Name);

    return audioFiles;
}

bool MediaFileSystem::isFolderContainingAudio(QDir folder)
{
    QStringList audioExtensionFilters;
    audioExtensionFilters << "*.mp3";
    audioExtensionFilters << "*.flac";
    audioExtensionFilters << "*.FLAC";
    audioExtensionFilters << "*.wav";
    audioExtensionFilters << "*.WAV";
    audioExtensionFilters << "*.ogg";
    audioExtensionFilters << "*.OGG";

    QStringList audioFiles = folder.entryList(audioExtensionFilters, QDir::Files, QDir::Name);

    qDebug() << folder.absolutePath() << " contains " << audioFiles.size() << " files";

    return ! audioFiles.empty();
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
        // TODO: Is there a reason for this..?
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
        qDebug() << "Generating from root";
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

        qDebug() << "Applying options to folder -> " << folderPath;
        next->applyFolderOptions(QDir(folderPath));

        mQmlMediaItems.append((QObject *)next);
    }

    mEngine->rootContext()->setContextProperty("MediaList", QVariant::fromValue(mQmlMediaItems));

}

void MediaFileSystem::createSaikIndex(bool pRecheck)
{
    QString path;
    path = mLibraryAbsPaths.at(0);

    QDirIterator itr(path, QDir::Dirs | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);

    while(itr.hasNext())
    {
        itr.next();
        QDir currentDir(itr.filePath());

        QStringList acceptedImageExtensions;
        acceptedImageExtensions << "*.jpg";
        acceptedImageExtensions << "*.png";

        QStringList imageFiles = currentDir.entryList(acceptedImageExtensions, QDir::Files, QDir::Name);

        if(imageFiles.size() == 0) {
            qDebug() << "No suitable images found for " << currentDir.absolutePath();
            continue;
        }

        if(currentDir.exists(".saik"))
        {
            qDebug() << ".saik folder alreay exists for : " << currentDir.absolutePath();

            if(!pRecheck)
                continue;
        }

        QDir tempDir;

        if(!tempDir.exists(itr.filePath() + "/.saik")) {
            tempDir.mkdir(itr.filePath() + "/.saik");
            qDebug() << "Creating '" << itr.filePath() + "/.saik" << "'";
        }

        QDir saikFolder(itr.filePath() + "/.saik");

        if(!saikFolder.exists("config.saik")) {
            QFile saikFile(saikFolder.absolutePath() + "/config.saik");
            qDebug() << "Creating '" << saikFolder.absolutePath() + "/config.saik'";
        }

        QFile saikConfigFile(saikFolder.absolutePath() + "/config.saik");

        if(!saikConfigFile.open(QIODevice::ReadWrite)) {
            qDebug() << "Failed to open " << saikConfigFile.fileName();
            continue;
        }

        QTextStream out(&saikConfigFile);

        foreach(QString imageName, imageFiles)
        {
            out << "front_cover:" << imageName << endl;
            qDebug() << "Appending " + imageName + " to saik file";
            break; // TODO: remove or do something
        }

        out.flush();
        saikConfigFile.close();
    }

    qDebug() << "Successfully created all saik files";
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


