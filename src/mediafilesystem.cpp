#include "mediafilesystem.h"

MediaFileSystem::MediaFileSystem(const QList<kfs::DirectoryPath>& library_roots, QQmlApplicationEngine *engine, QObject *parent)
    :   QObject(parent),
        m_qt_engine {engine},
        m_root_library_directories {library_roots},
        m_library_view_directory {std::nullopt},
        m_library_view_depth {0},
        m_audio_list_directory {std::nullopt},
        m_library_index {-1},
        m_playlist {},
        m_audio_list_files {},
        m_library_media_items {},
        m_audio_image_path {std::nullopt},
        m_current_audio_index {-1},
        m_error_message {}
{
//    for(const auto& root_dir : m_root_library_directories) {
//        m_library_media_items << MediaItem { root_dir.leafName(), false };
//    }

    MediaItem first { "Audio", false };
    m_library_media_items.append(first);

    m_library_view_qml.append(&m_library_media_items.front());

    m_qt_engine->rootContext()->setContextProperty(QML_LIBRARY_VIEW_NAME, QVariant::fromValue(m_library_view_qml));

    loadAudioListToQmlContext();

    emit playlistIndexChanged(m_current_audio_index);
//    loadLibraryViewContent();
}

void MediaFileSystem::nextTrack()
{
    if(m_current_audio_index >= m_audio_list_files.size() - 1) {
        return;
    }

    m_current_audio_index++;

    emit playlistIndexChanged(m_current_audio_index);
    emit currentAudioChanged(m_audio_list_files[m_current_audio_index]);
}

void MediaFileSystem::prevTrack()
{
    if(m_current_audio_index <= 0) {
        return;
    }

    m_current_audio_index--;

    emit playlistIndexChanged(m_current_audio_index);
    emit currentAudioChanged(m_audio_list_files[m_current_audio_index]);
}

// NEW API BEGIN

void MediaFileSystem::displayErrorMessage(QString message)
{
    qDebug() << "displayError: " << message;
    m_error_message = message;
    emit isErrorMessageChanged(true);
}

bool MediaFileSystem::atRootDirectory()
{
    return m_library_view_depth == 0;
}

void MediaFileSystem::cdLibraryRoot()
{
    m_library_index = -1;
    m_library_view_depth = 0;
    m_library_view_directory = std::nullopt;

    emit libraryViewDirChanged();
}

void MediaFileSystem::cdDown(const kfs::RelativePath& dir)
{
    if(m_library_view_directory == std::nullopt) {
        displayErrorMessage("Unknown error: Switching to root directory");
        cdLibraryRoot();
    } else if (!m_library_view_directory.value().cd( dir.path() )) {
        displayErrorMessage("Couldn't find directory '" + dir.path() + "'");
    }

    m_library_view_depth++;
    updateAudioFolderImagePath(dir.path());
    loadLibraryViewContent();
    emit libraryViewDirChanged();
}

void MediaFileSystem::updateAudioFolderImagePath(QString dir_name)
{
    for(auto& item : m_library_media_items) {
        if(item.getItemName() == dir_name) {
            m_audio_image_path = item.getImagePath();
            return;
        }
    }

    displayErrorMessage("Failed to update audio image");
}

void MediaFileSystem::cdDown(const QString& root_name)
{
    u32 library_index = 0;
    for(const auto& library_path : m_root_library_directories)
    {
        if(library_path.leafName() == root_name)
        {
            m_library_index = library_index;
            m_library_view_directory = std::optional<QDir>( {library_path.absolutePath()} );
            m_library_view_depth = 1;
            loadLibraryViewContent();
            return;
        }
        library_index++;
    }

    displayErrorMessage("Couldn't find library '" + root_name + "'");
}

void MediaFileSystem::appendTrackToPlaylist(kfs::FileIdentifier file)
{
    TagLib::Tag* file_tag = kpl::audio::extractTag(file);
    int32_t track_seconds = kpl::audio::extractLengthSeconds(file);

    QTime track_duration(0, track_seconds / 60, track_seconds % 60, 0);

    QString title = kpl::audio::extractTitle(file_tag);
    QString artist = kpl::audio::extractArtist(file_tag);
    QString file_name = file.fullName();

    AudioFile audio_file(title, artist, file_name, track_duration);

    m_playlist.append(std::move(audio_file));

//    emit playlistChanged(m_playlist);
}

// TODO: Playlist isn't exposed to QML yet
void MediaFileSystem::makeCurrentFolderPlaylist()
{
    QStringList audio_file_names = MediaFileSystem::audioInDir(*m_library_view_directory);
    kfs::DirectoryPath dir_path {*m_library_view_directory};
    m_playlist = AudioFile::fromFileNames(dir_path, audio_file_names);
//    loadAudioListToQmlContext();
}

void MediaFileSystem::loadAudioListToQmlContext()
{
//    if(m_audio_list_files.isEmpty()) {
//        return;
//    }

    QObjectList audio_list_qml;

    for(auto& audio : m_audio_list_files) {
        audio_list_qml.append(&audio);
    }

    qDebug() << "here..";

    m_qt_engine->rootContext()->setContextProperty(QML_AUDIO_VIEW_NAME, QVariant::fromValue(audio_list_qml));
//    mEngine->rootContext()->setContextProperty(QML_AUDIO_VIEW_NAME, &m_playlist.front());

    qDebug() << "Loaded " << audio_list_qml.size() << " files";
}

void MediaFileSystem::loadLibraryViewItemsToQmlContext()
{
    m_library_view_qml.clear();

    for(auto& item : m_library_media_items) {
        m_library_view_qml.append(&item);
    }

    m_qt_engine->rootContext()->setContextProperty(QML_LIBRARY_VIEW_NAME, QVariant::fromValue(m_library_view_qml));
}

QString MediaFileSystem::getLibraryViewDirectoryName()
{
    if(m_library_view_depth == 0) {
        return "Your Music Library";
    }

    return m_library_view_directory->dirName();
}

QString MediaFileSystem::getAudioViewDirectoryName()
{
    if(m_audio_list_directory == std::nullopt) {
        return "";
    }

    return m_audio_list_directory->dirName();
}

AudioFile MediaFileSystem::getCurrentAudio()
{
    if(m_current_audio_index < 0 || m_playlist.isEmpty() || m_current_audio_index >= m_playlist.size()) {
        return AudioFile { "", "", "", QTime(0, 0, 0, 0) };
    }

    return m_playlist[m_current_audio_index];
}

// NEW API END

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

void MediaFileSystem::cdUp()
{
    if(m_library_view_depth == 0) {
        qDebug() << "At root";
        return;
    }

    m_library_view_depth--;
    m_library_view_directory.value().cdUp();

    loadLibraryViewContent();

    qDebug() << "New Depth -> " << m_library_view_depth;

    emit libraryViewDirChanged();

//    m_library_media_items
}

void MediaFileSystem::loadAudioList()
{
    if(m_audio_list_directory == std::nullopt) {
        displayErrorMessage("Error: Not directory selected for audio");
        return;
    }

    QStringList audio_file_names = MediaFileSystem::audioInDir(*m_audio_list_directory);
    m_audio_list_files = AudioFile::fromFileNames(*m_audio_list_directory, audio_file_names);

    assert(m_audio_list_files.size() > 0);

    loadAudioListToQmlContext();

    qDebug() << "Audio list loaded";
}

void MediaFileSystem::invokeFolder(QString folder_name)
{
    qDebug() << "Invoking D:" << folder_name;

    if(m_library_view_directory == std::nullopt) {
        cdDown(folder_name);
        return;
    }

    std::optional<kfs::RelativePath> child_path_opt = kfs::RelativePath::make(folder_name);

    if(child_path_opt == std::nullopt) {
        displayErrorMessage("Error: Failed to cd down, invalid path");
        return;
    }

    QDir child_dir = makeChildDir(*m_library_view_directory, folder_name);

    if(dirContainsAudio(child_dir)) {
        m_audio_list_directory = child_dir;
        loadAudioList();
    } else {
        cdDown(child_path_opt.value());
    }


//    if(m_library_view_directory == std::nullopt) {
//        for(const auto& root : m_root_library_directories) {
//            if(root.leafName() == folder_name) {
//                qDebug() << "Entering root dir " << root.absolutePath();
//                m_library_view_directory = root.absolutePath();
//                loadLibraryViewItemsToQmlContext();
//                return;
//            }
//        }
//    } else if (dirContainsAudioRecursive(makeChildDir(*m_library_view_directory, folder_name))) {
//        qDebug() << "cdDown";
//        cdDown(folder_name);
//    }

//    qDebug() << "Here";

//    if(dirContainsAudio(makeChildDir(*m_library_view_directory, folder_name)))
//    {
//        qDebug() << "Contains audio";
//        m_audio_list_directory = makeChildDir(*m_library_view_directory, folder_name);

//        QStringList audio_file_names = MediaFileSystem::audioInDir(*m_audio_list_directory);
//        m_audio_list_files = AudioFile::fromFileNames(*m_library_view_directory, audio_file_names);
//        loadAudioListToQmlContext();

//        libraryViewDirChanged();
//    }
}

//void MediaFileSystem::invokeMediaItem(QString pFileName, QString pExtension)
//{
//    qDebug() << "Invoking media item";

//    if(!mCurrentDir || !mCurrentDir->exists())
//    {
//        downDir(pFileName);
//        return;
//    }

//    if(pExtension != "") {
//        generatePlaylist(pFileName + "." + pExtension);
//    } else {

//        if(hasAudioContainingSubFolders(makeChildDir(*mCurrentDir, pFileName))) {
//            downDir(pFileName);
//        }

//        if(isFolderContainingAudio(makeChildDir(*mCurrentDir, pFileName))) {
//            qDebug() << "Loading audio files";

//            mAudioFolder = makeChildDir(*mCurrentDir, pFileName);
//            loadAudioFromFolder(makeChildDir(*mCurrentDir, pFileName));
//            currentAudioFolderChanged();
//        }
//    }
//}

void MediaFileSystem::invokeAudioListing(int audio_index)
{
    qDebug() << "Audio index " << audio_index;

    if(audio_index < 0 || audio_index >= m_audio_list_files.size()) {
        qDebug() << "Invalid audio index passed to invokeAudioListing";
        return;
    }

    m_current_audio_index = audio_index;

    emit currentAudioChanged(m_audio_list_files[m_current_audio_index]);
    emit playlistIndexChanged(m_current_audio_index);
}

//void MediaFileSystem::playFromCurrentAudioSelection(unsigned long index)
//{
//    if(mAudioFolder == std::nullopt) {
//        qDebug() << "No audio folder set";
//        return;
//    }

//    // TODO:
////    QDir temp = *mCurrentDir;
////    qDebug() << "Getting images for " << temp.absolutePath();
////    QStringList selectedImages = getBestImagesPaths(temp, 4, {50,50});

////    std::optional<Magick::Image> tiledImageOpt = createTiledImage(selectedImages, {200, 200});

////    if(tiledImageOpt != std::nullopt)
////    {
////        Magick::Image tiledImage = tiledImageOpt.value();
////        tiledImage.magick("jpg");

////        if(!QDir(mCurrentDir->absolutePath() + "/.saik").exists()) {
////            QDir().mkdir(mCurrentDir->absolutePath() + "/.saik");
////        }

////        QString saveLocation = mCurrentDir->absolutePath() + "/.saik/tiled_artist_art.jpg";
////        tiledImage.write(saveLocation.toUtf8().data());

////        qDebug() << "Saving image to " << saveLocation;
////    } else {
////        qDebug() << "Failed to create tiled image";
////    }

//    mPlaylist.clear();

//    for(unsigned long i = index; i < mCurrentFolderAudioFiles.size(); i++) {
//        assert(mCurrentFolderAudioFiles[i].getFileName() != "");
//        mPlaylist.append(mAudioFolder.value().absolutePath() + "/" + mCurrentFolderAudioFiles[i].getFileName());
//    }

//    emit playlistChanged(mPlaylist);
//}

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

uint16_t MediaFileSystem::availableImagesInSubFolders(QDir directory)
{
    QStringList acceptedImageExtensions;
    acceptedImageExtensions << "*.jpg";
    acceptedImageExtensions << "*.png";

    uint16_t result = 0;

    QDirIterator itr(directory.absolutePath(), QDir::Files | QDir::NoDotAndDotDot, QDirIterator::NoIteratorFlags);

    while(itr.hasNext())
    {
        itr.next();
        QDir currentDir(itr.filePath());

        QStringList imagesInFolder = currentDir.entryList(acceptedImageExtensions, QDir::Files, QDir::Name);
        result += imagesInFolder.size();
    }

    return result;
}

bool imageIsValid(QString imagePath)
{
    qDebug() << "Checking " << imagePath << "... ";

    bool result = false;

    try {
        result = Magick::Image(imagePath.toUtf8().data()).isValid() ? true : false;
    } catch (...) {
        return false;
    }

    if(result) {
        qDebug() << "success";
    } else {
        qDebug() << "Invalid";
    }

    return result;
}

QString MediaFileSystem::bestResolution(QString first, QString second, Resolution targetRes)
{
    Magick::Image firstImage(first.toUtf8().data());

    Magick::Image secondImage(second.toUtf8().data());

    uint32_t firstWidth = static_cast<uint32_t>(firstImage.size().width());
    uint32_t firstHeight = static_cast<uint32_t>(firstImage.size().height());

    uint32_t secondWidth = static_cast<uint32_t>(secondImage.size().width());
    uint32_t secondHeight = static_cast<uint32_t>(secondImage.size().height());

    int64_t firstWidthDifference = firstWidth - targetRes.width;
    int64_t firstHeightDifference = firstHeight - targetRes.height;

    int64_t secondWidthDifference = secondWidth - targetRes.width;
    int64_t secondHeightDifference = secondHeight - targetRes.height;

    if(firstWidthDifference < 0) {
        firstWidthDifference = static_cast<int64_t>(static_cast<double>(firstWidthDifference) * -1.3);
    }

    if(firstHeightDifference < 0) {
        firstHeightDifference = static_cast<int64_t>(static_cast<double>(firstHeightDifference) * -1.3);
    }

    if(secondWidthDifference < 0) {
        secondWidthDifference = static_cast<int64_t>(static_cast<double>(secondWidthDifference) * -1.3);
    }

    if(secondHeightDifference < 0) {
        secondHeightDifference = static_cast<int64_t>(static_cast<double>(secondHeightDifference) * -1.3);
    }

    int64_t firstRating = firstWidthDifference + firstHeightDifference;
    int64_t secondRating = secondWidthDifference + secondHeightDifference;

    return (firstRating < secondRating) ? first : second;
}

// Make optional
QString MediaFileSystem::bestImageOf(QDir directory, QStringList images, Resolution res)
{
    if(images.size() == 0) {
        return "";
    }

    QString currentBest = directory.absolutePath() + "/" + images[0];

    for(auto& image : images) {
        currentBest = bestResolution(currentBest, directory.absolutePath() + "/" + image, res);
    }

    return currentBest;
}

std::optional<Magick::Image> MediaFileSystem::createTiledImage(QStringList sourceImagesPaths, Resolution res)
{
    if(sourceImagesPaths.size() == 0) {
        return std::nullopt;
    }

    qDebug() << "Images given -> " << sourceImagesPaths.size();

    uint16_t imagesPerSide = static_cast<uint16_t>(sqrt(sourceImagesPaths.size()));

    if(imagesPerSide * imagesPerSide < sourceImagesPaths.size()) {
        imagesPerSide++;
    }

    qDebug() << "Images per size -> " << imagesPerSide;

    uint32_t subImageWidth = res.width / imagesPerSide;
    uint32_t subImageHeight = res.height / imagesPerSide;

//    assert(subImageWidth == 100 && subImageHeight == 100 && imagesPerSide <= 2);

    qDebug() << "Sub Image Width -> " << subImageWidth;
    qDebug() << "Sub Image Height -> " << subImageHeight;
    qDebug() << "Images per side -> " << imagesPerSide;

    Magick::Geometry subImageGeometry(subImageWidth, subImageHeight);
    subImageGeometry.aspect(true);

    std::vector<Magick::Image> sourceImages;

    for(int i = 0; i < sourceImagesPaths.size(); i++) {
        sourceImages.push_back(Magick::Image(sourceImagesPaths[i].toUtf8().data()));
        sourceImages.back().resize(subImageGeometry);
    }

    qDebug() << "Resize complete";

    // TODO: Use res
    Magick::Image tiledImage("200x200", "grey");

    uint32_t sourceImageIndex = 0;
    bool forwardDirection = true;

    for(uint32_t y = 0; y < imagesPerSide; y++)
    {
        for(uint32_t x = 0; x < imagesPerSide; x++)
        {
            if(sourceImageIndex > 0 && sourceImageIndex % sourceImages.size() == 0) {
                if(sourceImageIndex % 2 == 0) {
                    forwardDirection = false;
                    sourceImageIndex--;
                } else {
                    sourceImageIndex = 0;
                }
            }

            tiledImage.composite(sourceImages[sourceImageIndex], Magick::Geometry(subImageWidth, subImageHeight, x * subImageWidth, y * subImageHeight));

            if(forwardDirection) {
                sourceImageIndex++;
            } else {
                sourceImageIndex--;
            }
        }
    }

    return std::optional<Magick::Image>(tiledImage);
}

QStringList MediaFileSystem::getBestImagesPaths(QDir directory, uint16_t numImages, Resolution res)
{
    QStringList acceptedImageExtensions;
    acceptedImageExtensions << "*.jpg";
    acceptedImageExtensions << "*.png";

    QStringList bestImages;

    QDirIterator itr(directory.absolutePath(), QDir::Dirs | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);

    while(itr.hasNext())
    {
        itr.next();
        QDir currentDir(itr.filePath());

        QStringList imagesInFolder = currentDir.entryList(acceptedImageExtensions, QDir::Files, QDir::Name);

        for(auto& image : imagesInFolder) {
            if(!imageIsValid(currentDir.absolutePath() + "/" + image)) {
                imagesInFolder.removeOne(image);
            }
        }

        if(imagesInFolder.size() == 0) {
            continue;
        } else if (bestImages.size() < numImages) {
            bestImages.append(bestImageOf(currentDir, imagesInFolder, {50, 50}));
        } else {

            QString bestImageInFolder = bestImageOf(currentDir, imagesInFolder, {50, 50});

            for(int i = 0; i < bestImages.size(); i++)
            {
                if(bestResolution(bestImages[i], bestImageInFolder, res) == bestImageInFolder) {
                    bestImages[i] = bestImageInFolder;
                    break;
                }
            }
        }
    }

    for(auto &image : bestImages) {
        qDebug() << "Selected Image: " << image;
    }

    return bestImages;
}

bool MediaFileSystem::dirContainsAudioRecursive(const QDir& folder)
{
    assert(folder.exists() && "hasAudioContainingSubFolders passed folder that doesn't exist");
    QStringList subFolders = folder.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);

    for(const auto& subFolder : subFolders) {
        qDebug() << "Checking ->" << subFolder;
        QDir currentFolder = makeChildDir(folder, subFolder);
        if(dirContainsAudio(currentFolder) || dirContainsAudioRecursive(currentFolder)) {
            qDebug() << "Has audio!";
            return true;
        }
    }

    qDebug() << folder.absolutePath() << " does not have audio";

    return false;
}

QDir MediaFileSystem::makeChildDir(const QDir& parent, const QString& childName)
{
    QDir result(parent.absolutePath() + "/" + childName);

    QString path_string = "makeChildDirFailed on " + parent.absolutePath() + "/" + childName;
    assert(result.exists() && path_string.data());

    return result;
}

QStringList MediaFileSystem::audioInDirRecursive(const QDir& folder)
{
    assert(folder.exists());

    QStringList subFolders = folder.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
    QStringList audioContainingSubFolders;

    for(const auto& subFolder : subFolders) {
        if(dirContainsAudioRecursive(makeChildDir(folder,subFolder))) {
            audioContainingSubFolders.append(subFolder);
        }
    }

    return audioContainingSubFolders;
}

QStringList MediaFileSystem::audioInDir(const QDir& folder)
{
    assert(folder.exists());

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

bool MediaFileSystem::dirContainsAudio(const QDir& folder)
{
    assert(folder.exists());

    qDebug() << "Check: " << folder.path() << " contains audio";

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

void MediaFileSystem::loadLibraryViewContent()
{
    m_library_media_items.clear();

    if(m_library_view_depth == 0)
    {
        m_library_media_items.reserve( m_root_library_directories.size() );

        for(const auto& root : m_root_library_directories) {
            m_library_media_items << MediaItem { root.leafName(), false };
        }

        loadLibraryViewItemsToQmlContext();
        return;
    }

    qDebug() << "Loaded non-root folder";
    QDirIterator itr(m_library_view_directory->absolutePath(), QDir::Dirs | QDir::NoDotAndDotDot, QDirIterator::NoIteratorFlags);

//    itr.next();

    while(itr.hasNext())
    {
        itr.next();
        QDir current_dir(itr.filePath());
        m_library_media_items << MediaItem {current_dir.dirName(), false };
        m_library_media_items.back().applyFolderOptions(current_dir);
    }

    loadLibraryViewItemsToQmlContext();
}

// *NOTE: We're assuming that the root folders only contain more folders atm
//void MediaFileSystem::generateMediaItemsFromRoot()
//{
//    MediaItem *next;
//    mQmlMediaItems.clear();
//    QStringList directories;

//    qDebug() << "libraryAbsPaths size : " << mLibraryAbsPaths.size();

//    for(int i = 0; i < mLibraryAbsPaths.size(); i++)
//    {
//        // TODO: Is there a reason for this..?
//        QDir *rootDir = new QDir(mLibraryAbsPaths.at(i));
//        if(!rootDir->exists())
//        {
//            qDebug() << mLibraryAbsPaths.at(i) << " doesn't exist";
//            delete rootDir;
//            continue;
//        }

//        directories = rootDir->entryList(QDir::AllDirs | QDir::NoDotAndDotDot, QDir::Name);

//        for(int libIndex = 0; libIndex < directories.size(); libIndex++)
//        {
//            QString dirFullPath = mLibraryAbsPaths.at(i) + "/" + directories.at(libIndex);
//            QDir currDir(dirFullPath);
//            if(!currDir.exists())
//            {
//                qDebug() << directories.at(libIndex) << " doesn't exist?";
//                continue;
//            }

//            QString dirName = getNameFromPath(dirFullPath);
//            QString imagePath = dirFullPath + "/" + dirName + "_art.jpg";
//            if(!currDir.exists(imagePath))
//            {
//                next = new MediaItem(dirFullPath, false);
//            }else
//            {
//                qDebug() << "Found Image: " << imagePath;
//                next = new MediaItem(dirFullPath, imagePath, false);
//            }

//            mQmlMediaItems.append(next);
//        }
//    }

//    mEngine->rootContext()->setContextProperty("MediaList", QVariant::fromValue(mQmlMediaItems));
//    qDebug() << "Root Media Items loaded successfuly";
//}

//void MediaFileSystem::generateMediaItems()
//{
//    qDebug() << "Generating media items..";

//    QStringList directories;
//    QStringList files;
//    MediaItem *next;
//    mQmlMediaItems.clear();

//    if(!mCurrentDir || !mCurrentDir->exists())
//    {
//        qDebug() << "Generating from root";
//        generateMediaItemsFromRoot();
//        return;
//    }

//    directories = mCurrentDir->entryList(QDir::AllDirs | QDir::NoDotAndDotDot, QDir::Name);

//    if(directories.size() <= 0)
//    {
//        qDebug() << "No directories found in current folder";
//        files = mCurrentDir->entryList(mFilters, QDir::Files, QDir::Name);

//        for(int i = 0; i < files.size(); i++)
//        {
//            qDebug() << "F: " << files.at(i) << " found";
//            next = new MediaItem(files.at(i), true);

//            mQmlMediaItems.append((QObject *)next);
//        }
//    }

//    for(int i = 0; i < directories.size(); i++)
//    {
//        qDebug() << "D: " << directories.at(i) << " found";

//        //QString folderPath = mLibraryAbsPaths.at(mCurrentLibraryIndex) + directories.at(i) + "/";
//        QString folderPath = mCurrentDir->absolutePath() + "/" + directories.at(i) + "/";
//        QStringList imageNameList = extractFolderImagePaths(folderPath);
//        QString imagePath;

//        for(int i = 0; i < imageNameList.size(); i++)
//        {
//            QDir folderDir(folderPath);
//            if(folderDir.exists(imageNameList.at(i)))
//            {
//                imagePath = folderPath + "/" + imageNameList.at(i);
//                qDebug() << imagePath + " exists";
//            }
//        }

//        if(imagePath.size() != 0)
//        {
//            next = new MediaItem(directories.at(i), imagePath, false);
//        }else
//        {
//            next = new MediaItem(directories.at(i), false);
//        }

//        qDebug() << "Applying options to folder -> " << folderPath;
//        next->applyFolderOptions(QDir(folderPath));

//        mQmlMediaItems.append((QObject *)next);
//    }

//    mEngine->rootContext()->setContextProperty("MediaList", QVariant::fromValue(mQmlMediaItems));
//}

void MediaFileSystem::generateSaikoMetaData(kfs::DirectoryPath root_dir, bool recheck)
{
    qDebug() << "generateSaikoMetaData disabled";
    return;

    QString path = root_dir.absolutePath();

    QDirIterator itr(path, QDir::Dirs | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);

    // Skip root
    itr.next();

    while(itr.hasNext())
    {
        itr.next();
        QDir currentDir(itr.filePath());

        if(currentDir.exists(".saik"))
        {
            qDebug() << ".saik folder alreay exists for : " << currentDir.absolutePath();

            if(!recheck) {
                continue;
            }
        }

        bool isAudioFolder = dirContainsAudio(currentDir);

        if(!isAudioFolder && !dirContainsAudioRecursive(currentDir)) {
            qDebug() << "Skipping " << currentDir.absolutePath();
            continue;
        }

        QStringList acceptedImageExtensions;
        acceptedImageExtensions << "*.jpg";
        acceptedImageExtensions << "*.png";

        uint16_t targetImages = isAudioFolder ? 1 : 4;
        QStringList imageFiles;

        if(isAudioFolder)
        {
            QStringList imagesInFolder = currentDir.entryList(acceptedImageExtensions, QDir::Files, QDir::Name);

            for(auto& image : imagesInFolder) {
                if(!imageIsValid(currentDir.absolutePath() + "/" + image)) {
                    imagesInFolder.removeOne(image);
                }
            }

            imageFiles.append(bestImageOf(currentDir, imagesInFolder, {150, 150}).split('/').back());
        } else
        {
            assert(imageFiles.empty());
            imageFiles.append(getBestImagesPaths(currentDir, targetImages, {150,150}));
        }

        if(imageFiles.size() == 0) {
            qDebug() << "No suitable images found for " << currentDir.absolutePath();
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

        QString imageLocation;

        if(!isAudioFolder)
        {
            qDebug() << "Generating a tiled image from album art";

            std::optional<Magick::Image> tiledImageOpt = createTiledImage(imageFiles, {200, 200});

            qDebug() << "Image created..";

            if(tiledImageOpt != std::nullopt)
            {
                Magick::Image tiledImage = tiledImageOpt.value();
                tiledImage.magick("jpg");

                imageLocation = currentDir.absolutePath() + "/.saik/tiled_artist_art.jpg";
                qDebug() << "Saving image to " << imageLocation;
                tiledImage.write(imageLocation.toUtf8().data());

                imageLocation = ".saik/tiled_artist_art.jpg";
            } else {
                qDebug() << "ERR: Failed to create tiled image";
                saikConfigFile.close();
                continue;
            }
        } else {
            imageLocation = imageFiles[0];
        }

        qDebug() << "Image location -> " << imageLocation;

        QTextStream out(&saikConfigFile);
        out << "front_cover:" << imageLocation << endl;

        out.flush();
        saikConfigFile.close();

        qDebug() << "Written to file";
    }

    qDebug() << "Successfully created all saik files";
}

void MediaFileSystem::purgeSaikData(const kfs::DirectoryPath& path)
{
    qDebug() << "purgeSaikData disabled";
    return;

    QDirIterator itr(path.absolutePath(), QDir::AllDirs | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);

    while(itr.hasNext())
    {
        itr.next();
        QDir currentDir(itr.filePath());

        QDir saikFolder(currentDir.absolutePath() + "/.saik/");

        if(!saikFolder.exists()) {
            continue;
        }

        qDebug() << "Removing -> " << saikFolder.absolutePath();
        saikFolder.removeRecursively();
    }

    qDebug() << "Successfully removed all saik data";
}
