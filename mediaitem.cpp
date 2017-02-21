#include "mediaitem.h"

MediaItem::MediaItem(QObject *parent) : QObject(parent)
{
    Q_UNUSED(parent);
    qDebug() << "Please don't call this..";
}

MediaItem::MediaItem(QString item_path, bool canPlay, QObject *parent) : QObject(parent)
{
    QChar sep('/');
    itemName = item_path.section(sep, -1, -1);
    isPlayable = canPlay;
}

MediaItem::MediaItem(QString item_path, QString image_path, bool canPlay, QObject *parent) : QObject(parent)
{
    QChar sep('/');
    itemName = item_path.section(sep, -1, -1);
    imagePath = image_path;
    imagePath.prepend("file:///");
    isPlayable = canPlay;
}

MediaItem::~MediaItem()
{
    qDebug() << "Freeing MediaItem";
}

void MediaItem::setItemName(QString newItemName)
{
    itemName = newItemName;
}

void MediaItem::setImagePath(QString newImagePath)
{
    imagePath = newImagePath;
}

void MediaItem::setIsPlayable(bool newIsPlayable)
{
    isPlayable = newIsPlayable;
}
