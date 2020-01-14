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

#include <mediadirectory.h>
#include <audioplaylist.h>

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
constexpr const char * QML_PLAYLISTS_NAME = "Playlists";

class Option : public QObject
{
    Q_OBJECT
public:


private:
    QString m_description;
    QString m_icon_path;
    bool    m_enabled;
};

class MediaFileSystem : public QObject
{
    Q_OBJECT

public:

//    Q_PROPERTY(QString libraryViewDirPath READ getLibraryViewDirectoryPath NOTIFY libraryViewDirChanged)
    Q_PROPERTY(QString libraryViewDirName READ getLibraryViewDirectoryName NOTIFY libraryViewDirChanged)

//    Q_PROPERTY(QString audioViewDirPath READ getAudioViewDirPath NOTIFY audioViewDirChanged)
    Q_PROPERTY(QString audioViewDirName READ getAudioViewDirectoryName NOTIFY audioViewDirChanged)

    Q_PROPERTY(int numberItemsLibraryView READ getNumberItemsLibraryView NOTIFY numberItemsLibraryViewChanged)

//    Q_PROPERTY(QString backendErrorMessage READ getErrorMessage)
//    Q_PROPERTY(bool isErrorMessage READ getIsErrorMessage NOTIFY isErrorMessageChanged)

//    Q_PROPERTY(QString ongoingTaskName READ getOngoingTaskName NOTIFY ongoingTaskNameChanged)
//    Q_PROPERTY(bool isOngoingTask READ getIsOngoingTask NOTIFY isOngoingTaskChanged)
//    Q_PROPERTY(double ongoingTaskProgress READ getOngoingTaskProgress NOTIFY ongoingTaskProgressChanged)

    Q_PROPERTY(int currentPlaylistIndex READ getCurrentPlaylistIndex NOTIFY playlistIndexChanged)
    Q_PROPERTY(AudioFile * currentAudio READ getCurrentAudio NOTIFY currentAudioChanged)

    Q_PROPERTY(bool restoreLibraryViewPosition READ getRestoreLibraryViewPosition WRITE setRestoreLibraryViewPosition)
    Q_PROPERTY(QString currentAudioImagePath READ getCurrentAudioImagePath NOTIFY audioImagePathChanged)
    Q_PROPERTY(bool audioFolderViewIsCurrent READ getAudioFolderViewIsCurrent NOTIFY audioFolderViewIsCurrentChanged)

    Q_PROPERTY(bool editMode READ getEditMode WRITE setEditMode NOTIFY editModeChanged)
    Q_PROPERTY(bool toolTipsEnabled READ getToolTipsEnabled WRITE setToolTipsEnabled NOTIFY toolTipsEnabledChanged)
    Q_PROPERTY(bool isHomeDirectory READ atRootDirectory NOTIFY isHomeDirectoryChanged)
    Q_PROPERTY(bool audioListTrayOpen READ isAudioListTrayOpen WRITE setIsAudioListTrayOpen NOTIFY isAudioListTrayOpenChanged)

    // TODO: move to .cpp
    bool getAudioFolderViewIsCurrent() {
        return m_audio_list_directory->directory() == m_playlist_directory->directory();
    }

signals:
    void isErrorMessageChanged(bool);
    void libraryViewDirChanged();
    void editModeChanged(bool);
    void toolTipsEnabledChanged(bool);
    void isHomeDirectoryChanged(bool);
    void isAudioListTrayOpenChanged(bool);

    void audioFolderViewIsCurrentChanged(bool);

    void audioViewDirChanged(QString);
    void currentAudioChanged(AudioFile);

    void ongoingTaskNameChanged(QString);
    void isOngoingTaskChanged(bool);
    void ongoingTaskProgressChanged(double);

    void audioImagePathChanged(QString);

    void musicPlaylistChanged(QList<AudioFile>);
    void playlistIndexChanged(int);
    void numberItemsLibraryViewChanged(int);

public:

    void setIsAudioListTrayOpen(bool is_open){
        m_audio_tray_open = is_open;
        emit isAudioListTrayOpenChanged(is_open);
    }

    bool isAudioListTrayOpen(){ return m_audio_tray_open; }

    // TODO: Move to .cpp
    void setEditMode(bool edit_mode){ m_edit_mode_enabled = edit_mode; emit editModeChanged(m_edit_mode_enabled); }
    bool getEditMode() const { return m_edit_mode_enabled; }

    void setToolTipsEnabled(bool enabled){ m_tool_tips_enabled = enabled; emit toolTipsEnabledChanged(enabled); }
    bool getToolTipsEnabled(){ return m_tool_tips_enabled; }

    int getRestoreLibraryViewPosition();
    void setRestoreLibraryViewPosition(bool restore_position);

    // TODO: Implement properly and move to .cpp
    Q_INVOKABLE void addLibraryRootPath(QString path){
        path = path.right(path.size() - 7);

        m_root_library_directories.append( *kfs::DirectoryPath::make(path) );

        if(!dirContainsAudioRecursive(path)) {
            displayErrorMessage("Library root path contains no audio");
            m_root_library_directories.pop_back();
            return;
        }

        loadLibraryViewContent();
        qDebug() << "Complete!";
    }

    // TODO: Tell QML so that it can update the model
    Q_INVOKABLE void addTrackToPlaylist(int playlist_index, int track_index);
    Q_INVOKABLE void removeTrackFromPlaylist(int playlist_index, int track_index);
    Q_INVOKABLE void invokePlaylist(int playlist_index);

    void loadPlaylistsToQML();

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

    Q_INVOKABLE void regenerateSaikForParentLibView();
    Q_INVOKABLE void generateSaikoInCurrentLibView(QString item_name);
    Q_INVOKABLE void generateSaikForCurrentLibView();
    Q_INVOKABLE void purgeSaikForCurrentLibView();

    void generateSaikoMetaDataRecursive(kfs::DirectoryPath root_dir, bool recheck);
    void generateSaikoMetaData(kfs::DirectoryPath root_dir, bool recheck);
    void purgeSaikData(const kfs::DirectoryPath& path);

    Q_INVOKABLE int getNumberItemsLibraryView();
    Q_INVOKABLE QString getCurrentAudioImagePath() {

        if(m_playlist_index != -1) {
            qDebug() << "Image Path -> " << m_playlists[m_playlist_index].getTracks()[m_current_audio_index].m_art_path;
            return m_playlists[m_playlist_index].getTracks()[m_current_audio_index].m_art_path;
        }

        if(m_playlist_directory == std::nullopt || m_playlist_directory->imagePath() == "") {
            return "qrc:///resources/cover.jpg";
        }

        return m_playlist_directory->imagePath();
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

    void loadLibraryViewContent();

    AudioFile * getCurrentAudio();
    void loadAudioList();

    void loadLibraryViewItemsToQmlContext();
    void loadAudioListToQmlContext();

    static bool dirContainsAudio(const QDir& dir);
    static QStringList audioInDir(const QDir& dir);

    static bool dirContainsAudioRecursive(const QDir& dir);
    static QStringList audioInDirRecursive(const QDir& dir);

    /* Member Variables */

    QQmlApplicationEngine *     m_qt_engine;
    QList<kfs::DirectoryPath>   m_root_library_directories;

    std::optional<QDir>         m_library_view_directory;

    std::array<AudioPlaylist, 4>    m_playlists;
    i32                             m_playlist_index = -1;

    u32                         m_library_view_depth;

    std::optional<MediaDirectory>   m_audio_list_directory;
    std::optional<MediaDirectory>   m_playlist_directory;

//    QString                     m_album_image_path;
    i32                         m_library_index;
    QList<int>                  m_saved_library_view_position;

    /*  Exposed to QML engine       */
    QList<AudioFile>            m_playlist;
    QList<AudioFile>            m_audio_list_files;
    QList<MediaItem>            m_library_media_items;

//    std::optional<QString>      m_audio_image_path;
    i32                         m_current_audio_index;

    bool                        m_restore_library_view_position;
    bool                        m_edit_mode_enabled = false;
    bool                        m_tool_tips_enabled = false;
    bool                        m_audio_tray_open = false;

    // TODO: This shouldn't be needed
    QObjectList                 m_library_view_qml;

//    QStringList                 m_supported_audio_extensions;
//    QStringList                 m_supported_image_extensions;

    QString                     m_error_message;

    // NEW API END
public:
    MediaFileSystem(const QList<kfs::DirectoryPath>& library_roots, QQmlApplicationEngine *engine, QObject *parent = nullptr);

    // TODO: Move to util class
    static QDir makeChildDir(const QDir& parent, const QString& childName);

private:

    // TODO: Remove to utility layer
    static QString loadAlbumArtToFileIfExists(QString filePath, QString successPath, QString failurePath);
    QString getNameFromPath(QString pPath);
    QStringList extractFolderImagePaths(QString pCurrentAbsPath);
    static uint16_t availableImagesInSubFolders(QDir directory);
    static QStringList getBestImagesPaths(QDir directory, uint16_t numImages, Resolution res);
    static QString bestResolution(QString first, QString second, Resolution targetRes);
    static QString bestImageOf(QDir directory, QStringList images, Resolution res);
    static std::optional<Magick::Image> createTiledImage(const kfs::DirectoryPath& root_dir, QStringList sourceImagesPaths, Resolution res);
};

#endif // MEDIAFILESYSTEM_H
