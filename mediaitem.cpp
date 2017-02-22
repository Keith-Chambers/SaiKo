#include "mediaitem.h"

MediaItem::MediaItem(QString pItemPath, bool pIsPlayable, QObject *parent) : QObject(parent)
{
    QChar sep('/');
    mItemName = pItemPath.section(sep, -1, -1);
    mIsPlayable = pIsPlayable;
}

MediaItem::MediaItem(QString pItemPath, QString pImagePath, bool pIsPlayable, QObject *parent) : QObject(parent)
{
    QChar sep('/');
    mItemName = pItemPath.section(sep, -1, -1);
    mImagePath = pImagePath;
    mImagePath.prepend("file:///");
    mIsPlayable = pIsPlayable;
}

MediaItem::~MediaItem()
{
    qDebug() << "Freeing MediaItem";
}

void MediaItem::setItemName(QString pItemName)
{
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
