#include "audiofile.h"

AudioFile::AudioFile(QString title, QString artist, QString file_name, QTime duration)
    :   m_title     {   title       },
        m_artist    {   artist      },
        m_art_path  {   ""          },
        m_file_name {   file_name   },
        m_duration  {   duration    }
{}

AudioFile::AudioFile(AudioFile&& audio_file)
    :   m_title     { std::move(audio_file.m_title)     },
        m_artist    { std::move(audio_file.m_artist)    },
        m_art_path  { std::move(audio_file.m_art_path)  },
        m_file_name { std::move(audio_file.m_file_name) },
        m_duration  { std::move(audio_file.m_duration)  }
{}

AudioFile::AudioFile(const AudioFile& copy)
    :   QObject     {   nullptr             },
        m_title     {   copy.m_title        },
        m_artist    {   copy.m_artist       },
        m_art_path  {   copy.m_art_path     },
        m_file_name {   copy.m_file_name    },
        m_duration  {   copy.m_duration     }

{}

AudioFile& AudioFile::operator=(const AudioFile& copy)
{
    m_title     =   copy.m_title;
    m_artist    =   copy.m_artist;
    m_art_path  =   copy.m_art_path;
    m_file_name =   copy.m_file_name;
    m_duration  =   copy.m_duration;

    return *this;
}

QString AudioFile::getTitle()
{
    return m_title;
}

QString AudioFile::getArtist()
{
    return m_artist;
}

QString AudioFile::getArtPath()
{
    return m_art_path;
}

QString AudioFile::getFileName()
{
    return m_file_name;
}

bool AudioFile::getHasArt()
{
    return m_art_path != "";
}

void AudioFile::setFileName(QString file_name)
{
    m_file_name = file_name;
}

void AudioFile::setTitle(QString title)
{
    m_title = title;
}

void AudioFile::setArtist(QString artist)
{
    m_artist = artist;
}

void AudioFile::setArtPath(QString artPath)
{
    m_art_path = artPath;
}

std::optional<AudioFile> AudioFile::fromFileName(const kfs::FileIdentifier& file)
{
    TagLib::FileRef f(file.absolutePath().toUtf8());
    TagLib::Tag* file_tag = f.tag();

    if(!file_tag) {
        return std::nullopt;
    }

    int32_t track_seconds = kpl::audio::extractLengthSeconds(file);

    qDebug() << "Track length (seconds) : " << track_seconds;

    QTime track_duration(0, track_seconds / 60, track_seconds % 60, 0);

    QString title = kpl::audio::extractTitle(file_tag);

    qDebug() << "Title : " << title;

    QString artist = kpl::audio::extractArtist(file_tag);
    QString file_name = file.absolutePath();

    return std::optional<AudioFile>(AudioFile(title, artist, file_name, track_duration));
}

QList<AudioFile> AudioFile::fromFileNames(const QList<kfs::FileIdentifier>& files)
{
    QList<AudioFile> audio_files;
    audio_files.reserve(files.size());

    for(const auto& file : files)
    {
        std::optional<AudioFile> audio_file_opt = fromFileName( file );

        if(audio_file_opt != std::nullopt) {
            audio_files.append( std::move(*audio_file_opt) );
        }
    }

    return audio_files;
}

QList<AudioFile> AudioFile::fromFileNames(const kfs::DirectoryPath& parent_path, const QStringList& file_names)
{
    QList<AudioFile> audio_files;
    audio_files.reserve(file_names.size());

    QString prefix_path = parent_path.absolutePath();

    qDebug() << "Path prefix: " << prefix_path;

    for(const auto& file : file_names)
    {
        qDebug() << "Audio file: " << file;
        std::optional<kfs::FileIdentifier> complete_file_opt = kfs::FileIdentifier::make( parent_path, file );

        if(complete_file_opt == std::nullopt) {
            qDebug() << "Skipping";
            continue;
        }

        std::optional<AudioFile> audio_file_opt = fromFileName( *complete_file_opt );

        if(audio_file_opt != std::nullopt) {
            qDebug() << "Appending";
            audio_files.append( *audio_file_opt );
        }
    }

    return audio_files;
}

QTime AudioFile::getDuration()
{
    return m_duration;
}
