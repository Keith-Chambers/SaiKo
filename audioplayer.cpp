#include "audioplayer.h"

AudioPlayer::AudioPlayer(QObject *parent)
    : QObject(parent), mSoundStopEvent(), mCurrentAudio("Unknown", "Unknown")
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
    mPositionTimer->start(250);

    mPlaylistIndex = 0;
}

AudioPlayer::~AudioPlayer(void)
{
    mCurrentSound->drop();
    mEngine->drop();
}

AudioFile * AudioPlayer::getCurrentAudio()
{
    return &mCurrentAudio;
}

void AudioPlayer::AudioPlayer::setPlaylist(QStringList pPlaylist)
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

int AudioPlayer::getCurrentSongIndex()
{
    return mPlaylistIndex;
}

QString AudioPlayer::loadAlbumArtToFileIfExists(QString filePath, QString successPath, QString failurePath)
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
                qDebug() << "Loop begin";
                PicFrame = (TagLib::ID3v2::AttachedPictureFrame *)(*it);

                if(PicFrame->type() == TagLib::ID3v2::AttachedPictureFrame::FrontCover)
                {
                    // extract image (in it's compressed form)
                    Size = PicFrame->picture().size();
                    SrcImage = malloc ( Size );

                    if(SrcImage)
                    {
                        FILE *jpegFile = fopen(successPath.toStdString().c_str(), "wb");

                        if(!jpegFile) {
                            return failurePath;
                        }

                        memcpy(SrcImage, PicFrame->picture().data(), Size) ;
                        fwrite(SrcImage, Size, 1, jpegFile);
                        fclose(jpegFile);

                        free(SrcImage);

                        return successPath;
                    }
                }
                qDebug() << "Loop end";
            }
        }
    }

    qDebug() << "Failed to write album art";

    return failurePath;
}

QString AudioPlayer::getAudioTitle(QFile audioFile)
{
    TagLib::FileRef f(audioFile.fileName().toStdString().c_str());
    TagLib::Tag *tag = f.tag();

    return QString::fromStdString(std::string(tag->title().to8Bit()));

//    TagLib::String artist = f.tag()->artist(); // artist == "Frank Zappa"
//    f.tag()->setAlbum("Fillmore East");
//    f.save();
//    TagLib::FileRef g("Free City Rhymes.ogg");
//    TagLib::String album = g.tag()->album(); // album == "NYC Ghosts & Flowers"
//    g.tag()->setTrack(1);
//    g.save();
}

QString AudioPlayer::getAudioArtist(QFile audioFile)
{
    TagLib::FileRef f(audioFile.fileName().toStdString().c_str());
    TagLib::Tag *tag = f.tag();

    return QString::fromStdString(std::string(tag->artist().to8Bit()));
}

QString AudioPlayer::getAudioAlbum(QFile audioFile)
{
    TagLib::FileRef f(audioFile.fileName().toStdString().c_str());
    TagLib::Tag *tag = f.tag();

    return QString::fromStdString(std::string(tag->album().to8Bit()));
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

    QFile audioFile = QFile(mPlaylist.at(mPlaylistIndex));

    qDebug() << "Setting title";
    QString title = getAudioTitle(QFile(mPlaylist.at(mPlaylistIndex)));

    if(title == "") {
        title = QFileInfo(QFile(mPlaylist.at(mPlaylistIndex))).fileName();
    }

    QString artist = getAudioArtist(QFile(mPlaylist.at(mPlaylistIndex)));

    if(artist == "") {
        artist = "Unknown";
    }

    QString album = getAudioAlbum(QFile(mPlaylist.at(mPlaylistIndex)));

    if(album == "") {
        album = "Unknown";
    }

    QString albumArtLocation = "file:/home/keith/current_album_art.jpg";

    QDir parentDir(mPlaylist.at(mPlaylistIndex));
    parentDir.cdUp();

    QString failurePath = frontImageForFolder(parentDir);

    if(failurePath == "") {
        failurePath = "qrc:///resources/cover.jpg";
    } else {
        failurePath.prepend("file:");
    }

    qDebug() << "Setting Art";
    QString artPath = loadAlbumArtToFileIfExists(mPlaylist.at(mPlaylistIndex), albumArtLocation, failurePath);
    qDebug() << "Art path -> " << artPath;

    qDebug() << "File name -> " << title;

    mCurrentAudio.setTitle(title);
    mCurrentAudio.setArtist(artist);
    mCurrentAudio.setArtPath(artPath);
    qDebug() << "Done";

    mCurrentSound = mEngine->play2D(mPlaylist.at(mPlaylistIndex).toUtf8(), false, true);
    mCurrentSound->setSoundStopEventReceiver((irrklang::ISoundStopEventReceiver*) &mSoundStopEvent, this);

    // TODO: Introduce a delay to prevent skipping
    mCurrentSound->setIsPaused(false);

    emit isPlayingChanged(true);
    emit songChanged();

    qDebug() << "Music playing";
}

void AudioPlayer::setCurrentAudio(QString fileName)
{

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
