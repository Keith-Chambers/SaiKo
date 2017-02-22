#include "audioplayer.h"

AudioPlayer::AudioPlayer(QObject *parent) : QObject(parent), soundStopEvent()
{
    qDebug() << "AudioPlayer Constructer invoked";

    m_curr_sound = Q_NULLPTR;
    m_engine = irrklang::createIrrKlangDevice();

    if (!m_engine)
    {
        printf("Could not startup engine\n");
    }

    playlistIndex = 0;
}

AudioPlayer::~AudioPlayer(void)
{
    m_curr_sound->drop();
    m_engine->drop();
}

void AudioPlayer::setPlaylist(QStringList playlist)
{
    this->playlist.clear();
    this->playlist = playlist;
    playlistIndex = 0;

    setupMusic();
}

void AudioPlayer::setupMusic(void)
{
    qDebug() << "Setting up music";

    if(m_curr_sound)
    {
        m_curr_sound->drop();
        m_curr_sound = Q_NULLPTR;
    }


    if(playlist.size() == 0)
    {
        m_curr_sound = Q_NULLPTR;
        return;
    }

    if(playlistIndex >= playlist.size() || playlistIndex < 0)
    {
        qDebug() << "Error playlist size = " << playlist.size() << " and playlistIndex = " << playlistIndex;
        m_curr_sound = Q_NULLPTR;
        return;
    }

    m_curr_sound = m_engine->play2D(playlist.at(playlistIndex).toUtf8(), false, true);
    m_curr_sound->setSoundStopEventReceiver((irrklang::ISoundStopEventReceiver*) &soundStopEvent, this);
    m_curr_sound->setIsPaused(false);
}

void AudioPlayer::setPlayPosition(double pos)
{
    unsigned long currPos = m_curr_sound->getPlayPosition();
    currPos = (double)currPos * pos;
    m_curr_sound->setPlayPosition(currPos);

    qDebug() << "Audio position changed";
}

double AudioPlayer::getPlayPosition(void)
{
    unsigned long fullLength = m_curr_sound->getPlayLength();
    unsigned long currPos = m_curr_sound->getPlayPosition();

    return (double)currPos / (double)fullLength;
}

void AudioPlayer::nextSong(void)
{
    if((playlistIndex + 1) >= playlist.size())
        return;

    playlistIndex++;
    setupMusic();
    qDebug() << "nextSong invoked";
}

void AudioPlayer::prevSong(void)
{
    if(playlistIndex - 1 < 0)
        return;

    playlistIndex--;
    setupMusic();
    qDebug() << "prevSong invoked";
}

void AudioPlayer::togglePause(void)
{
    if(m_engine->getSoundSourceCount() == 0 || !m_curr_sound)
        return;

    if(m_curr_sound->getIsPaused())
        m_curr_sound->setIsPaused(false);
    else
        m_curr_sound->setIsPaused(true);

    qDebug() << "togglePause invoked";
}

bool AudioPlayer::getIsPlaying(void)
{
    if(!m_curr_sound)
        return false;
    return m_curr_sound->getIsPaused();
}

void AudioPlayer::setIsPlaying(bool isPlaying)
{
    if(!m_curr_sound)
        return;
    m_curr_sound->setIsPaused(isPlaying);
}

void AudioPlayer::stopAudio(void)
{
    qDebug() << "stopAudio invoked";
}
