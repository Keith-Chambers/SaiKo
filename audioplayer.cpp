#include "audioplayer.h"

AudioPlayer::AudioPlayer(QObject *parent) : QObject(parent), mSoundStopEvent()
{
    qDebug() << "AudioPlayer Constructer invoked";

    mCurrentSound = Q_NULLPTR;
    mEngine = irrklang::createIrrKlangDevice();

    if (!mEngine)
    {
        printf("Could not startup engine\n");
    }

    mPositionTimer = new QTimer(this);
    connect(mPositionTimer, &QTimer::timeout, this, &AudioPlayer::updateAudioPosition);
    mPositionTimer->start(1000);

    mPlaylistIndex = 0;
}

AudioPlayer::~AudioPlayer(void)
{
    mCurrentSound->drop();
    mEngine->drop();
}

void AudioPlayer::setPlaylist(QStringList pPlaylist)
{
    mPlaylist.clear();
    mPlaylist = pPlaylist;

    qDebug() << "Playlist:";
    for(int i = 0; i < mPlaylist.size(); i++) {
        qDebug() << mPlaylist[i];
    }

    mPlaylistIndex = 0;

    playMusic();
}

void AudioPlayer::playMusic(void)
{
    qDebug() << "Setting up music";

    if(mCurrentSound) {
        qDebug() << "Stopping current sound";
        mCurrentSound->stop();
    }

    if(mPlaylist.size() == 0)
    {
        qDebug() << "Playlist empty";
        mCurrentSound = Q_NULLPTR;
        return;
    }

    if(mPlaylistIndex >= mPlaylist.size() || mPlaylistIndex < 0)
    {
        qDebug() << "Error playlist size = " << mPlaylist.size() << " and playlistIndex = " << mPlaylistIndex;
        mCurrentSound = Q_NULLPTR;
        return;
    }

    // TODO: You can set pause to false here if you set the forth param

    qDebug() << "Playing: " << mPlaylist.at(mPlaylistIndex).toUtf8();

    mCurrentSound = mEngine->play2D(mPlaylist.at(mPlaylistIndex).toUtf8(), false, true);
    mCurrentSound->setSoundStopEventReceiver((irrklang::ISoundStopEventReceiver*) &mSoundStopEvent, this);

    // TODO: Introduce a delay to prevent skipping
    mCurrentSound->setIsPaused(false);

    emit isPlayingChanged(true);

    qDebug() << "Music playing";
}

void AudioPlayer::updateAudioPosition()
{
    emit playPositionChanged( getPlayPosition() );
}

void AudioPlayer::setPlayPosition(double pPos)
{
    unsigned int endPos = mCurrentSound->getPlayLength();
    unsigned int currPos = static_cast<unsigned int>(endPos * pPos);

    mCurrentSound->setPlayPosition(currPos);

    qDebug() << "Audio position changed to " << currPos / endPos;
}

double AudioPlayer::getPlayPosition(void)
{
    if(!mCurrentSound) {
        return 0.0;
    }

    unsigned long fullLength = mCurrentSound->getPlayLength();
    unsigned long currPos = mCurrentSound->getPlayPosition();

    return (double)currPos / (double)fullLength;
}

void AudioPlayer::nextSong(void)
{
    if((mPlaylistIndex + 1) >= mPlaylist.size())
        return;

    mPlaylistIndex++;
    playMusic();
    qDebug() << "nextSong invoked";
}

void AudioPlayer::prevSong(void)
{
    if(mPlaylistIndex - 1 < 0)
        return;

    mPlaylistIndex--;
    playMusic();
    qDebug() << "prevSong invoked";
}

void AudioPlayer::togglePause(void)
{
    if(mEngine->getSoundSourceCount() == 0 || !mCurrentSound)
        return;

    if(mCurrentSound->getIsPaused())
        mCurrentSound->setIsPaused(false);
    else
        mCurrentSound->setIsPaused(true);

    emit isPlayingChanged( ! mCurrentSound->getIsPaused() );

    qDebug() << "togglePause invoked";
}

bool AudioPlayer::getIsPlaying(void)
{
    if(!mCurrentSound)
        return false;

    return !mCurrentSound->getIsPaused();
}

void AudioPlayer::setIsPlaying(bool pIsPlaying)
{
    if(!mCurrentSound)
        return;
    mCurrentSound->setIsPaused(pIsPlaying);
}

void AudioPlayer::stopAudio(void)
{
    qDebug() << "stopAudio invoked";
}
