#ifndef AUDIOFILE_H
#define AUDIOFILE_H

#include <QObject>
#include <QString>
#include <QTime>
#include <QDebug>

#include <optional>

#include <kpl/filesystem.h>
#include <kpl/audiofile.h>

namespace kfs = kpl::filesystem;

// Add track length, etc
class AudioFile : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString title READ getTitle NOTIFY titleChanged)
    Q_PROPERTY(QString artist READ getArtist NOTIFY artistChanged)
    Q_PROPERTY(QString artPath READ getArtPath NOTIFY artPathChanged)
    Q_PROPERTY(QTime duration READ getDuration)
    Q_PROPERTY(bool hasArt READ getHasArt NOTIFY hasArtChanged)

    Q_PROPERTY(QString audioLength READ getAudioLength)

public:

    AudioFile(){}

    AudioFile(  QString     title,
                QString     artist,
                QString     file_name,
                QTime       duration );


    AudioFile(const AudioFile& copy);
    AudioFile(AudioFile&& audioFile);
    AudioFile& operator=(const AudioFile& copy);

    Q_INVOKABLE QString getAudioLength(){ return m_duration.toString("mm:ss"); }

    static std::optional<AudioFile> fromFileName(const kfs::FileIdentifier& file);
    static QList<AudioFile> fromFileNames(const QList<kfs::FileIdentifier>& file_names);
    static QList<AudioFile> fromFileNames(const kfs::DirectoryPath& parent_path, const QStringList& file_names);

    void setTitle(QString title);
    void setArtist(QString artist);
    void setArtPath(QString art_path);
    void setFileName(QString file_name);

    QString getTitle();
    QString getArtist();
    QString getArtPath();
    QString getFileName();
    bool getHasArt();
    QTime getDuration();

    // Member Variables
    QString     m_title;
    QString     m_artist;
    QString     m_art_path;
    QString     m_file_name;
    QTime       m_duration;

signals:
    void titleChanged(QString);
    void artistChanged(QString);
    void artPathChanged(QString);
    void hasArtChanged(bool);
};

Q_DECLARE_METATYPE(AudioFile)

#endif // AUDIOFILE_H
