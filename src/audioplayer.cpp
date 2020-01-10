#include "audioplayer.h"

AudioPlayer::AudioPlayer(QObject *parent)
    : QObject(parent),
      m_sound_stop_event(),
      m_volume {1.0},
      m_update_audio_position_timer {},
      m_current_audio {std::nullopt}
{
    m_irrklang_sound = Q_NULLPTR;
    m_irrklang_engine = irrklang::createIrrKlangDevice();

    if(!m_irrklang_engine)
    {
        qDebug() << "Failed to initialize IrrKlang audio engine";
        assert(false);
    }

    connect(&m_update_audio_position_timer, &QTimer::timeout, this, &AudioPlayer::updateAudioPosition);
    m_update_audio_position_timer.start(250);
}

AudioPlayer::~AudioPlayer()
{
    // TODO: This is seg faulting on shutdown
    m_irrklang_sound->drop();
    m_irrklang_engine->drop();
}

void AudioPlayer::playAudio(AudioFile audio)
{
    if(m_irrklang_sound) {
        qDebug() << "Stopping current sound";
        m_irrklang_sound->stop();
        m_irrklang_sound->drop();
        m_irrklang_sound = nullptr;
    }

    m_current_audio = kpl::filesystem::FileIdentifier::make(audio.getFileName());

    if(m_current_audio == std::nullopt) {
        qDebug() << audio.getFileName() << " doesn't exist";
        return;
    }

    m_irrklang_sound = m_irrklang_engine->play2D(m_current_audio->absolutePath().toUtf8(), false, true);

    if(!m_irrklang_sound) {
        qDebug() << "Failed to init irrklang sound";
        return;
    }

    m_irrklang_sound->setSoundStopEventReceiver(reinterpret_cast<irrklang::ISoundStopEventReceiver*>(&m_sound_stop_event), this);

    // TODO: Introduce a delay to prevent skipping
    m_irrklang_sound->setVolume(m_volume);
    m_irrklang_sound->setIsPaused(false);

    emit isPlayingChanged(true);
    emit audioChanged(audio.getFileName());
}

void AudioPlayer::updateAudioPosition()
{
    emit playPositionChanged( getPlayPosition() );
}

void AudioPlayer::setPlayPosition(double pPos)
{
    unsigned int endPos = m_irrklang_sound->getPlayLength();
    unsigned int currPos = static_cast<unsigned int>(endPos * pPos);

    m_irrklang_sound->setPlayPosition(currPos);
}

double AudioPlayer::getPlayPosition(void)
{
    if(!m_irrklang_sound) {
        return 0.0;
    }

    if(!getIsPlaying()) {
        return m_saved_audio_position;
    }

    unsigned long fullLength = m_irrklang_sound->getPlayLength();
    unsigned long currPos = m_irrklang_sound->getPlayPosition();

    return (double)currPos / (double)fullLength;
}

void AudioPlayer::togglePause(void)
{
    if(m_irrklang_engine->getSoundSourceCount() == 0 || !m_irrklang_sound)
        return;

//    switch(m_irrklang_sound->getIsPaused())
//    {
//    case true:
//        m_saved_audio_position = getPlayPosition();
//        m_irrklang_sound->setIsPaused(false);
//        break;
//    case false:
//        m_saved_audio_position = getPlayPosition();
//        m_irrklang_sound->setIsPaused(true);
//        break;
//    }

    if(m_irrklang_sound->getIsPaused())
    {
        m_saved_audio_position = getPlayPosition();
        m_irrklang_sound->setIsPaused(false);
    } else
    {
        m_saved_audio_position = getPlayPosition();
        m_irrklang_sound->setIsPaused(true);
    }

    emit isPlayingChanged( ! m_irrklang_sound->getIsPaused() );
}

bool AudioPlayer::getIsPlaying(void)
{
    if(!m_irrklang_sound)
        return false;

    return !m_irrklang_sound->getIsPaused();
}

double AudioPlayer::getVolume()
{
    if(!m_irrklang_sound) {
        return m_volume;
    }

    m_volume = m_irrklang_sound->getVolume();
    return m_volume;
}

void AudioPlayer::setVolume(double volume)
{
    if(volume > 1.0 || volume < 0.0) {
        return;
    }

    m_irrklang_sound->setVolume(volume);
    m_volume = volume;

    emit volumeChanged(volume);
}

void AudioPlayer::setPlayState(bool playing)
{
    if(!m_irrklang_sound) {
        qDebug() << "Warning: Setting isPlaying property but no audio loaded";
        return;
    }    

    m_irrklang_sound->setIsPaused(!playing);
}

void AudioPlayer::pauseAudio()
{
    m_saved_audio_position = getPlayPosition();
    setPlayState(false);
}

void AudioPlayer::resumeAudio()
{
    setPlayState(true);
}

