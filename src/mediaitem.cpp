#include "mediaitem.h"

MediaItem::MediaItem(QString pItemPath, bool pIsPlayable, QObject *parent) : QObject(parent)
{
    mIsPlayable = pIsPlayable;
    QChar sep('/');
    setItemName(pItemPath.section(sep, -1, -1));
}

MediaItem::MediaItem(QString pItemPath, QString pImagePath, bool pIsPlayable, QObject *parent) : QObject(parent)
{
    mIsPlayable = pIsPlayable;
    QChar sep('/');
    setItemName(pItemPath.section(sep, -1, -1));
    mImagePath = pImagePath;
    mImagePath.prepend("file:///");
}

MediaItem::~MediaItem()
{
    qDebug() << "Freeing MediaItem";
}

void MediaItem::setItemName(QString pItemName)
{

    // Removes file extension
    if(mIsPlayable)
    {
        QChar sep('.');

        mExtension = pItemName.section(sep, -1, -1);
        pItemName = pItemName.section(sep, 0, -2);
    }

    mItemName = pItemName;
}

void MediaItem::setImagePath(QString pImagePath)
{
    mImagePath = pImagePath;
}

void MediaItem::setIsPlayable(bool pIsPlayable)
{
    mIsPlayable = pIsPlayable;
}

void MediaItem::setExtension(QString pExtension)
{
    if(pExtension.size() > 4)
        return;
    mExtension = pExtension;
}
