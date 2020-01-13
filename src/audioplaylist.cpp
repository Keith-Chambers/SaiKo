#include "audioplaylist.h"

AudioPlaylist::AudioPlaylist(const QString name)
    : m_name {name}
{

}

int AudioPlaylist::getNumberTracks()
{
    return m_audio_files.size();
}

QString AudioPlaylist::getName()
{
    return m_name;
}

void AudioPlaylist::setName(QString name)
{
    m_name = name;
}

const QList<AudioFile>& AudioPlaylist::getTracks()
{
    return m_audio_files;
}

void AudioPlaylist::addTrack(const AudioFile& audio)
{
    m_audio_files.append(audio);
}

//void AudioPlaylist::addTrackUnique(const AudioFile& audio)
//{

//}

void AudioPlaylist::removeTrack(int index)
{
    m_audio_files.removeAt(index);
}
