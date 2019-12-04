#include "audiofile.h"

AudioFile::AudioFile(QString pName, QString pArtist)
    : mName {pName}, mArtist {pArtist}
{}

AudioFile::AudioFile(QString pName, QString pArtist, QImage* pArt)
    : mName {pName}, mArtist {pArtist}, mArt {pArt}
{}

QString AudioFile::getName()
{
    return mName;
}

QString AudioFile::getArtist()
{
    return mArtist;
}

QImage AudioFile::getArt()
{
    return *mArt;
}

bool AudioFile::getHasArt()
{
    return mArt != nullptr;
}
