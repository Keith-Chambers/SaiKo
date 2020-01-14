#include "mediafilesystem.h"

MediaFileSystem::MediaFileSystem(const QList<kfs::DirectoryPath>& library_roots, QQmlApplicationEngine *engine, QObject *parent)
    :   QObject(parent),
        m_qt_engine {engine},
        m_root_library_directories {library_roots},
        m_library_view_directory {std::nullopt},
        m_playlists { QString("A"), QString("B"), QString("C"), QString("D") },
        m_library_view_depth {0},
        m_audio_list_directory {std::nullopt},
        m_library_index {-1},
        m_playlist {},
        m_audio_list_files {},
        m_library_media_items {},
//        m_audio_image_path {std::nullopt},
        m_current_audio_index {-1},
        m_error_message {}
{

    for(const auto& root_dir : m_root_library_directories) {
        m_library_media_items << MediaItem { root_dir.leafName(), false };
        m_library_view_qml.append( &m_library_media_items.back() );
    }

    m_qt_engine->rootContext()->setContextProperty(QML_LIBRARY_VIEW_NAME, QVariant::fromValue(m_library_view_qml));

    loadAudioListToQmlContext();
    loadPlaylistsToQML();

    emit playlistIndexChanged(m_current_audio_index);
}

int MediaFileSystem::popLibraryViewPosition()
{
    int pos = m_saved_library_view_position.back();
    m_saved_library_view_position.pop_back();
    return pos;
}

void MediaFileSystem::regenerateSaikForParentLibView()
{
    if(m_library_view_directory == std::nullopt) {
        // TODO: Error handling
        return;
    }

    auto parent = m_library_view_directory;
    parent->cdUp();

    // TODO: Make this better
    generateSaikoMetaDataRecursive(kfs::DirectoryPath::make(parent->absolutePath()).value(), true);

    loadLibraryViewContent();
}

void MediaFileSystem::generateSaikoInCurrentLibView(QString item_name)
{
    qDebug() << "Generating Saik for " << item_name << " in current lib view";

    if(m_library_view_directory == std::nullopt) {
        // TODO: Error handling
        qDebug() << "Invalid library view";
        return;
    }

    // TODO: Make this better
    generateSaikoMetaData(kfs::DirectoryPath::make(m_library_view_directory->absolutePath() + "/" + item_name).value(), true);
    generateSaikoMetaDataRecursive(kfs::DirectoryPath::make(m_library_view_directory->absolutePath() + "/" + item_name).value(), true);

    qDebug() << "Reloading library";

    loadLibraryViewContent();
}

void MediaFileSystem::generateSaikForCurrentLibView()
{
    qDebug() << "Generating Saik for current lib view";

    if(m_library_view_directory == std::nullopt) {
        // TODO: Error handling
        qDebug() << "Invalid library view";
        return;
    }

    // TODO: Make this better
    generateSaikoMetaDataRecursive(kfs::DirectoryPath::make(m_library_view_directory->absolutePath()).value(), true);

    qDebug() << "Reloading library @ " << m_library_view_directory->absolutePath();

    loadLibraryViewContent();
}

void MediaFileSystem::purgeSaikForCurrentLibView()
{
    if(m_library_view_directory == std::nullopt) {
        // TODO: Error handling
        return;
    }

    // TODO: Make this better
    auto current_path = kfs::DirectoryPath::make(m_library_view_directory->path());
    purgeSaikData(current_path.value());

    loadLibraryViewContent();
}

Q_INVOKABLE int MediaFileSystem::getNumberItemsLibraryView()
{
    return m_library_media_items.size();
}

int MediaFileSystem::getRestoreLibraryViewPosition()
{
    return m_restore_library_view_position;
}

void MediaFileSystem::setRestoreLibraryViewPosition(bool restore_position)
{
    m_restore_library_view_position = restore_position;
}

void MediaFileSystem::pushLibraryViewPosition(int pos)
{
    qDebug() << "Saving restore index -> " << pos;
    m_saved_library_view_position.append(pos);
}

void MediaFileSystem::addTrackToPlaylist(int playlist_index, int track_index)
{
    if(playlist_index > 3 || playlist_index < 0) {
        qDebug() << "Invalid playlist index";
        return;
    }

    // Add to playlists / audio work space
    m_playlists[playlist_index].addTrack(m_audio_list_files[track_index]);

    // Append to playing playlist
    m_playlist.append(m_playlists[playlist_index].getTracks().back());

    QString image_path = m_audio_list_directory->imagePath();
    m_playlists[playlist_index].getTracks().back().m_art_path = image_path;

    loadPlaylistsToQML();
}

void MediaFileSystem::removeTrackFromPlaylist(int playlist_index, int track_index)
{
    if(playlist_index > 3 || playlist_index < 0) {
        qDebug() << "Invalid playlist index";
        return;
    }

    m_playlists[playlist_index].removeTrack(track_index);
    m_playlist.removeAt(track_index);

    loadPlaylistsToQML();
}

void MediaFileSystem::invokePlaylist(int playlist_index)
{
    if(playlist_index > 3 || playlist_index < 0) {
        qDebug() << "Invalid playlist index";
        return;
    }

    m_playlist_index = playlist_index;
    m_audio_list_files = m_playlists[playlist_index].getTracks();
    m_audio_list_directory = { {"Playlist 1"}, "" };

    emit audioViewDirChanged("Playlist 1");
    emit audioFolderViewIsCurrentChanged( getAudioFolderViewIsCurrent() );

    loadAudioListToQmlContext();
}

void MediaFileSystem::nextTrack()
{
    if(m_current_audio_index >= m_playlist.size() - 1) {
        return;
    }

    m_current_audio_index++;

    if(m_playlist_index != -1) {
        emit audioImagePathChanged(m_playlists[m_playlist_index].getTracks()[m_current_audio_index].getArtPath());
    }

    emit playlistIndexChanged(m_current_audio_index);
    emit currentAudioChanged(m_playlist[m_current_audio_index]);
}

void MediaFileSystem::prevTrack()
{
    if(m_current_audio_index <= 0) {
        return;
    }

    m_current_audio_index--;

    if(m_playlist_index != -1) {
        emit audioImagePathChanged(m_playlists[m_playlist_index].getTracks()[m_current_audio_index].getArtPath());
    }

    emit playlistIndexChanged(m_current_audio_index);
    emit currentAudioChanged(m_playlist[m_current_audio_index]);
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
    loadLibraryViewContent();
    emit libraryViewDirChanged();
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

void MediaFileSystem::loadPlaylistsToQML()
{
    QObjectList playlists_qml;

    for(auto& playlist : m_playlists) {
        playlists_qml.append(&playlist);
    }

    m_qt_engine->rootContext()->setContextProperty(QML_PLAYLISTS_NAME, QVariant::fromValue(playlists_qml));
}

void MediaFileSystem::loadAudioListToQmlContext()
{
    QObjectList audio_list_qml;

    for(auto& audio : m_audio_list_files) {
        audio_list_qml.append(&audio);
    }

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
    switch(m_playlist_index)
    {
        case 0:
            return "Playlist A";
        case 1:
            return "Playlist B";
        case 2:
            return "Playlist C";
        case 3:
            return "Playlist D";
        default:
            break;
    }

    if(m_audio_list_directory == std::nullopt) {
        return "";
    }

    return m_audio_list_directory->directory().dirName();
}

AudioFile* MediaFileSystem::getCurrentAudio()
{
    if(m_current_audio_index < 0 || m_playlist.isEmpty() || m_current_audio_index >= m_playlist.size()) {
        qDebug() << "Null audiofile";
        assert(false);
        return nullptr;
    }

    return &m_playlist[m_current_audio_index];
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

    QStringList audio_file_names = MediaFileSystem::audioInDir(m_audio_list_directory->directory());
    m_audio_list_files = AudioFile::fromFileNames(m_audio_list_directory->directory(), audio_file_names);

    assert(m_audio_list_files.size() > 0);

    loadAudioListToQmlContext();

    qDebug() << "Audio list loaded";
}

void MediaFileSystem::invokeFolder(QString folder_name)
{
    qDebug() << "Invoking D:" << folder_name;

    m_playlist_index = -1;

    if(m_library_view_directory == std::nullopt) {
        cdDown(folder_name);
        return;
    }

    std::optional<kfs::RelativePath> child_path_opt = kfs::RelativePath::make(folder_name);

    qDebug() << "Relative Path Invoked -> " << child_path_opt.value().path();

    if(child_path_opt == std::nullopt) {
        displayErrorMessage("Error: Failed to cd down, invalid path");
        return;
    }

    QDir child_dir = makeChildDir(*m_library_view_directory, folder_name);

    // IF: Is audio folder ELSE: Has more folders
    if(dirContainsAudio(child_dir))
    {
        popLibraryViewPosition();

        QString image_path = "qrc:///resources/cover.jpg";

        for(auto& item : m_library_media_items)
        {
            if(item.getItemName() == folder_name) {
                image_path = item.getImagePath();
                break;
            }
        }

        m_audio_list_directory = {child_dir, image_path};

        loadAudioList();
        audioViewDirChanged(m_audio_list_directory->directory().dirName());
    } else {
        cdDown(child_path_opt.value());
    }
}

void MediaFileSystem::invokeAudioListing(int audio_index)
{
    qDebug() << "Audio index " << audio_index;

    if(audio_index < 0 || audio_index >= m_audio_list_files.size()) {
        qDebug() << "Invalid audio index passed to invokeAudioListing";
        return;
    }

    m_playlist_directory = m_audio_list_directory;

    m_current_audio_index = audio_index;
    m_playlist = m_audio_list_files;

    emit currentAudioChanged(m_playlist[m_current_audio_index]);
    emit playlistIndexChanged(m_current_audio_index);
    emit audioImagePathChanged(m_playlist_directory->imagePath());
    emit audioFolderViewIsCurrentChanged( getAudioFolderViewIsCurrent() );
//    emit audioFolderViewIsCurrentChanged(true);
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

    qDebug() << "Best image of..";

    QString current_best = directory.absolutePath() + "/" + images[0];

    qDebug() << "First path -> " << current_best;

    uint16_t current_best_index = 0;

    for(uint16_t i = 1; i < images.size(); i++)
    {
        const auto image = images[i];

        qDebug() << "Compare with -> " << directory.absolutePath() + "/" + image;

        if(bestResolution(current_best, directory.absolutePath() + "/" + image, res) != current_best) {
            current_best_index = i;
            current_best = directory.absolutePath() + "/" + images[i];
        }
    }

//    for(auto& image : images) {
//        currentBest = bestResolution(currentBest, directory.absolutePath() + "/" + image, res);
//    }

    return images[current_best_index];
}

std::optional<Magick::Image> MediaFileSystem::createTiledImage(const kfs::DirectoryPath& root_dir, QStringList sourceImagesPaths, Resolution res)
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
        QString absolute_path = root_dir.absolutePath() + "/" + sourceImagesPaths[i];
        sourceImages.push_back(Magick::Image(absolute_path.toUtf8().data()));
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

QString relativeDifference(QDir parent, QDir child)
{
    QStringList parent_list = parent.absolutePath().split('/');
    QStringList child_list = child.absolutePath().split('/');

    assert(parent_list.size() <= child_list.size());

    if(parent_list.size() == child_list.size()) {
        return "";
    }

    QString result;

    for(uint16_t i = parent_list.size(); i < child_list.size(); i++) {
        result.append(child_list[i] + "/");
    }

    // Remove trailing '/'
    result.remove(result.size() - 1, 1);

    return result;
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

        QString relativePath = relativeDifference(directory.absolutePath(), currentDir.absolutePath());

        qDebug() << "Parent path -> " << directory.absolutePath();
        qDebug() << "Current path -> " << currentDir.absolutePath();
        qDebug() << "Relative difference -> " << relativePath;

        QStringList imagesInFolder = currentDir.entryList(acceptedImageExtensions, QDir::Files, QDir::Name);

        for(auto& image : imagesInFolder) {

            qDebug() << "Image: " << image;

            image.prepend(relativePath + "/");

            qDebug() << "New Image: " << image;

            qDebug() << "Checking -> " << directory.absolutePath() + "/" + image;
            if(!imageIsValid(directory.absolutePath() + "/" + image)) {
                imagesInFolder.removeOne(image);
            }
            qDebug() << "Valid";
        }

        if(imagesInFolder.size() == 0) {
            continue;
        } else if (bestImages.size() < numImages) {
            bestImages.append(bestImageOf(directory, imagesInFolder, {res.width,res.height}));
        } else {

            qDebug() << "Getting best images in folder";
            QString bestImageInFolder = bestImageOf(directory, imagesInFolder, {res.width, res.height});

            for(int i = 0; i < bestImages.size(); i++)
            {
                qDebug() << "Checking best resolution";

                if(bestResolution(directory.absolutePath() + "/" + bestImages[i], directory.absolutePath() + "/" + bestImageInFolder, res) == directory.absolutePath() + "/" + bestImageInFolder) {
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


    while(itr.hasNext())
    {
        itr.next();
        QDir current_dir(itr.filePath());
        m_library_media_items << MediaItem {current_dir.dirName(), false };
        m_library_media_items.back().applyFolderOptions(current_dir);
    }

    loadLibraryViewItemsToQmlContext();
}

void MediaFileSystem::generateSaikoMetaData(kfs::DirectoryPath root_dir, bool recheck)
{
    QDir target_folder(root_dir.absolutePath());

    if(!target_folder.exists()) {
        qDebug() << "Warning: Target folder doesn't exist for generateSaikoMetaData";
        return;
    }

    if(target_folder.exists(".saik") && recheck == false)
    {
        qDebug() << ".saik folder alreay exists for : " << root_dir.absolutePath();
        return;
    }

    bool is_audio_folder = dirContainsAudio(target_folder);

    if(!is_audio_folder && !dirContainsAudioRecursive(target_folder)) {
        qDebug() << "Not a valid directory for saik meta data " << target_folder.absolutePath();
        return;
    }

    QStringList acceptedImageExtensions;
    acceptedImageExtensions << "*.jpg";
    acceptedImageExtensions << "*.png";

    uint16_t targetImages = is_audio_folder ? 1 : 4;
    QStringList imageFiles;

    // TODO: Make more efficient
    uint16_t number_sub_folders = target_folder.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name).size();

    if(number_sub_folders == 1) {
        targetImages = 1;
    }

    qDebug() << "Number sub folders -> " << number_sub_folders;

    if(is_audio_folder)
    {
        QStringList imagesInFolder = target_folder.entryList(acceptedImageExtensions, QDir::Files, QDir::Name);

        for(auto& image : imagesInFolder) {
            if(!imageIsValid(target_folder.absolutePath() + "/" + image)) {
                imagesInFolder.removeOne(image);
            }
        }

        imageFiles.append(bestImageOf(target_folder, imagesInFolder, {150, 150}));
    } else
    {
        assert(imageFiles.empty());
        imageFiles.append(getBestImagesPaths(target_folder, targetImages, {150,150}));
    }

    if(imageFiles.size() == 0) {
        qDebug() << "No suitable images found for " << target_folder.absolutePath();
        return;
    }

    qDebug() << "At temp dir";

    QDir tempDir;

    if(!tempDir.exists(target_folder.absolutePath() + "/.saik")) {
        tempDir.mkdir(target_folder.absolutePath() + "/.saik");
        qDebug() << "Creating '" << target_folder.absolutePath() + "/.saik" << "'";
    }

    QDir saikFolder(target_folder.absolutePath() + "/.saik");

    qDebug() << "Target folder -> " << target_folder.absolutePath() + "/.saik";

    if(!saikFolder.exists("config.saik")) {
        QFile saikFile(saikFolder.absolutePath() + "/config.saik");
        qDebug() << "Creating '" << saikFolder.absolutePath() + "/config.saik'";
    }

    QFile saikConfigFile(saikFolder.absolutePath() + "/config.saik");

    if(!saikConfigFile.open(QIODevice::ReadWrite)) {
        qDebug() << "Failed to open " << saikConfigFile.fileName();
        return;
    }

    QString imageLocation;

    if(!is_audio_folder && imageFiles.size() != 1)
    {
        qDebug() << "Generating a tiled image from album art";

        std::optional<Magick::Image> tiledImageOpt = createTiledImage(root_dir, imageFiles, {200, 200});

        qDebug() << "Image created..";

        if(tiledImageOpt != std::nullopt)
        {
            Magick::Image tiledImage = tiledImageOpt.value();
            tiledImage.magick("jpg");

            imageLocation = target_folder.absolutePath() + "/.saik/tiled_artist_art.jpg";
            qDebug() << "Saving image to " << imageLocation;
            tiledImage.write(imageLocation.toUtf8().data());

            imageLocation = ".saik/tiled_artist_art.jpg";
        } else {
            qDebug() << "ERR: Failed to create tiled image";
            saikConfigFile.close();
            return;
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

void MediaFileSystem::generateSaikoMetaDataRecursive(kfs::DirectoryPath root_dir, bool recheck)
{
    QString path = root_dir.absolutePath();
    QDirIterator itr(path, QDir::Dirs | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);

    qDebug() << "Looping through folders to generate saiko data for path " << path;

    while(itr.hasNext())
    {
        itr.next();
        QDir currentDir(itr.filePath());

        std::optional<kfs::DirectoryPath> current_dir_opt = kfs::DirectoryPath::make(itr.path());

        if(current_dir_opt == std::nullopt) {
            qDebug() << "Failed to create path from " << itr.path();
            return;
        }

        qDebug() << "Generating for " << currentDir.absolutePath();

        generateSaikoMetaData(current_dir_opt.value(), recheck);
        continue;

        if(currentDir.exists(".saik"))
        {
            qDebug() << ".saik folder alreay exists for : " << currentDir.absolutePath();

            if(!recheck) {
                continue;
            }
        }

        bool is_audio_folder = dirContainsAudio(currentDir);

        if(!is_audio_folder && !dirContainsAudioRecursive(currentDir)) {
            qDebug() << "Skipping " << currentDir.absolutePath();
            continue;
        }

        QStringList acceptedImageExtensions;
        acceptedImageExtensions << "*.jpg";
        acceptedImageExtensions << "*.png";

        uint16_t targetImages = is_audio_folder ? 1 : 4;
        QStringList imageFiles;

        if(is_audio_folder)
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

        if(!is_audio_folder)
        {
            qDebug() << "Generating a tiled image from album art";

            std::optional<Magick::Image> tiledImageOpt = createTiledImage(root_dir, imageFiles, {200, 200});

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
