#ifndef MEDIAITEM_H
#define MEDIAITEM_H

#include <QObject>
#include <QImage>
#include <QString>
#include <QDebug>

class MediaItem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString itemName READ getItemName WRITE setItemName NOTIFY itemNameChanged)
    Q_PROPERTY(QString imagePath READ getImagePath WRITE setImagePath NOTIFY imagePathChanged)
    Q_PROPERTY(bool isPlayable READ getIsPlayable WRITE setIsPlayable NOTIFY isPlayableChanged)
public:
    explicit MediaItem(QObject *parent = 0);
    MediaItem(QString item_path, bool canPlay, QObject *parent = 0);
    MediaItem(QString item_path, QString image_path, bool canPlay, QObject *parent = 0);
    ~MediaItem();

    QString getItemName(){ return itemName; }
    QString getImagePath(){ return imagePath; }
    bool getIsPlayable(){ return isPlayable; }

    void setItemName(QString newItemName);
    void setImagePath(QString newDirImage);
    void setIsPlayable(bool newIsPlayable);
signals:
    void itemNameChanged(QString newItemName);
    void imagePathChanged(QString newDirImage);
    void isPlayableChanged(bool newIsPlayable);

public slots:

private:
    QString imagePath;
    QString itemName;
    bool isPlayable;
};

#endif // MEDIAITEM_H
