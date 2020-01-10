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
#include <QTime>

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

#include <kpl/filesystem.h>
#include <kpl/audiofile.h>

namespace kfs = kpl::filesystem;

#include "mediaitem.h"
#include "audiofile.h"
#include "textparser.h"

#include "audiofile.h"

// TODO: move
struct Resolution
{
    uint32_t width;
    uint32_t height;
};

typedef QStringList AbsoluteFilePathList;

constexpr const char * QML_PLAYLIST_VIEW_NAME = "PlaylistView";
constexpr const char * QML_AUDIO_VIEW_NAME = "AudioView";
constexpr const char * QML_LIBRARY_VIEW_NAME = "LibraryView";

class MediaFileSystem : public QObject
{
    Q_OBJECT

public:

//    Q_PROPERTY(QString currentFolderName READ getCurrentFolderName NOTIFY directoryChanged)
//    Q_PROPERTY(QString currentAudioFolderName READ getCurrentAudioFolderName NOTIFY currentAudioFolderChanged)

    // NEW API BEGIN

//    Q_PROPERTY(QString libraryViewDirPath READ getLibraryViewDirectoryPath NOTIFY libraryViewDirChanged)
    Q_PROPERTY(QString libraryViewDirName READ getLibraryViewDirectoryName NOTIFY libraryViewDirChanged)

//    Q_PROPERTY(QString audioViewDirPath READ getAudioViewDirPath NOTIFY audioViewDirChanged)
    Q_PROPERTY(QString audioViewDirName READ getAudioViewDirectoryName NOTIFY audioViewDirChanged)

//    Q_PROPERTY(QString backendErrorMessage READ getErrorMessage)
//    Q_PROPERTY(bool isErrorMessage READ getIsErrorMessage NOTIFY isErrorMessageChanged)

//    Q_PROPERTY(QString ongoingTaskName READ getOngoingTaskName NOTIFY ongoingTaskNameChanged)
//    Q_PROPERTY(bool isOngoingTask READ getIsOngoingTask NOTIFY isOngoingTaskChanged)
//    Q_PROPERTY(double ongoingTaskProgress READ getOngoingTaskProgress NOTIFY ongoingTaskProgressChanged)

//    Q_PROPERTY(QQmlProperty<AudioFile> musicPlaylist READ getMusicPlaylist NOTIFY musicPlaylistChanged)
    Q_PROPERTY(int currentPlaylistIndex READ getCurrentPlaylistIndex NOTIFY playlistIndexChanged)
    Q_PROPERTY(AudioFile * currentAudio READ getCurrentAudio NOTIFY currentAudioChanged)

//    Q_PROPERTY(QString audioImagePath READ getAudioImagePath NOTIFY audioImagePathChanged)
    Q_PROPERTY(bool restoreLibraryViewPosition READ getRestoreLibraryViewPosition WRITE setRestoreLibraryViewPosition)

//    Q_PROPERTY(int libraryViewPositionIndex READ getLibraryViewPositionIndex NOTIFY libraryViewPositionIndexChanged)

    //             onCurrentIndexChanged: {

//}

signals:
    void isErrorMessageChanged(bool);
    void libraryViewDirChanged();

    void audioViewDirChanged(QString);
    void currentAudioChanged(AudioFile);

    void ongoingTaskNameChanged(QString);
    void isOngoingTaskChanged(bool);
    void ongoingTaskProgressChanged(double);

    void audioImagePathChanged(QString);

    void musicPlaylistChanged(QList<AudioFile>);
    void playlistIndexChanged(int);

public:

    int getRestoreLibraryViewPosition();
    void setRestoreLibraryViewPosition(bool restore_position);

    Q_INVOKABLE int popLibraryViewPosition();
    Q_INVOKABLE void pushLibraryViewPosition(int pos);

    Q_INVOKABLE void cdUp();
//    Q_INVOKABLE void resetErrorMessage();
    Q_INVOKABLE void cdLibraryRoot();
    Q_INVOKABLE void invokeFolder(QString folder_name);
    Q_INVOKABLE void invokeAudioListing(int audio_index);
//    Q_INVOKABLE void cdToPlayingAudio();

    QString getLibraryViewDirectoryName();
//    QString getLibraryViewDirectoryPath();

    QString getAudioViewDirectoryName();

    Q_INVOKABLE void regenerateSaikForParentLibView()
    {
        if(m_library_view_directory == std::nullopt) {
            // TODO: Error handling
            return;
        }

        auto parent = m_library_view_directory;
        parent->cdUp();

        // TODO: Make this better
        generateSaikoMetaData(kfs::DirectoryPath::make(parent->absolutePath()).value(), true);

        loadLibraryViewContent();
    }

    // TODO: Move to .cpp
    Q_INVOKABLE void generateSaikForCurrentLibView()
    {
        if(m_library_view_directory == std::nullopt) {
            // TODO: Error handling
            return;
        }

        // TODO: Make this better
        generateSaikoMetaData(kfs::DirectoryPath::make(m_library_view_directory->absolutePath()).value(), true);

        loadLibraryViewContent();
    }

    Q_INVOKABLE void purgeSaikForCurrentLibView()
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

public slots:
    Q_INVOKABLE void nextTrack();
    Q_INVOKABLE void prevTrack();
private:

    void displayErrorMessage(QString message);
//    void displayWarningMessage(QString message);
//    void displayInfoMessage(QString message);

    bool atRootDirectory();

    int getCurrentPlaylistIndex(){ return m_current_audio_index; }

    void cdDown(const QString& root_name);
    void cdDown(const kfs::RelativePath& dir);
    void appendTrackToPlaylist(kfs::FileIdentifier file);
    void makeCurrentFolderPlaylist();

    void generateSaikoMetaData(kfs::DirectoryPath root_dir, bool recheck);
    void purgeSaikData(const kfs::DirectoryPath& path);

    void loadLibraryViewContent();

    // Updates m_parent_media_item with a MediaItem in the current directory
    void updateAudioFolderImagePath(QString dir_name);

    AudioFile * getCurrentAudio();
    void loadAudioList();

    void loadLibraryViewItemsToQmlContext();
    void loadAudioListToQmlContext();

    static bool dirContainsAudio(const QDir& dir);
    static QStringList audioInDir(const QDir& dir);

    static bool dirContainsAudioRecursive(const QDir& dir);
    static QStringList audioInDirRecursive(const QDir& dir);

    QQmlApplicationEngine *     m_qt_engine;
    QList<kfs::DirectoryPath>   m_root_library_directories;

    std::optional<QDir>         m_library_view_directory;
    u32                         m_library_view_depth;
    std::optional<QDir>         m_audio_list_directory;

//    QString                     m_album_image_path;
    i32                         m_library_index;
    QList<int>                  m_saved_library_view_position;

    /*  Exposed to QML engine       */
    QList<AudioFile>            m_playlist;
    QList<AudioFile>            m_audio_list_files;
    QList<MediaItem>            m_library_media_items;
    std::optional<QString>      m_audio_image_path;
    i32                         m_current_audio_index;

    bool                        m_restore_library_view_position;

    QObjectList                 m_library_view_qml;

//    QStringList                 m_supported_audio_extensions;
//    QStringList                 m_supported_image_extensions;

    QString                     m_error_message;

    // NEW API END
public:
    MediaFileSystem(const QList<kfs::DirectoryPath>& library_roots, QQmlApplicationEngine *engine, QObject *parent = nullptr);

//    void generateMediaItems();
//    void generateMediaItemsFromRoot();

    void purgeSaikData();

    // TODO: Move to util class
    static QDir makeChildDir(const QDir& parent, const QString& childName);

private:
    static QString loadAlbumArtToFileIfExists(QString filePath, QString successPath, QString failurePath);
    QString getNameFromPath(QString pPath);
    QStringList extractFolderImagePaths(QString pCurrentAbsPath);
    static uint16_t availableImagesInSubFolders(QDir directory);
    static QStringList getBestImagesPaths(QDir directory, uint16_t numImages, Resolution res);
    static QString bestResolution(QString first, QString second, Resolution targetRes);
    static QString bestImageOf(QDir directory, QStringList images, Resolution res);
    static std::optional<Magick::Image> createTiledImage(QStringList sourceImagesPaths, Resolution res);
};

#endif // MEDIAFILESYSTEM_H
