#include "audiofile.h"

AudioFile::AudioFile(QString pName, QString pArtist)
    : mTitle {pName}, mArtist {pArtist}, mArtPath {""}
{}

AudioFile::AudioFile(QString pName, QString pArtist, QString pArtPath)
    : mTitle {pName}, mArtist {pArtist}, mArtPath {pArtPath}
{}

AudioFile::AudioFile(AudioFile&& audioFile)
    : mTitle { std::move(audioFile.mTitle) }, mArtist { std::move(audioFile.mArtist) }, mArtPath { audioFile.mArtPath }
{}

QString AudioFile::getTitle()
{
    return mTitle;
}

QString AudioFile::getArtist()
{
    return mArtist;
}

QString AudioFile::getArtPath()
{
    return mArtPath;
}

bool AudioFile::getHasArt()
{
    return mArtPath != "";
}

void AudioFile::setTitle(QString title)
{
    mTitle = title;
}

void AudioFile::setArtist(QString artist)
{
    mArtist = artist;
}

void AudioFile::setArtPath(QString artPath)
{
    mArtPath = artPath;
}
