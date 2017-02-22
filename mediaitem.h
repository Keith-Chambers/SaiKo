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
    //TODO: Remove default constructor
    //explicit MediaItem(QObject *parent = 0);
    MediaItem(QString pItemPath, bool pIsPlayable, QObject *parent = 0);
    MediaItem(QString pItemPath, QString pImagePath, bool pIsPlayable, QObject *parent = 0);
    ~MediaItem();

    QString getItemName(){ return mItemName; }
    QString getImagePath(){ return mImagePath; }
    bool getIsPlayable(){ return mIsPlayable; }

    void setItemName(QString pItemName);
    void setImagePath(QString pDirImage);
    void setIsPlayable(bool pIsPlayable);
signals:
    void itemNameChanged(QString pItemName);
    void imagePathChanged(QString pDirImage);
    void isPlayableChanged(bool pIsPlayable);

public slots:

private:
    QString mImagePath;
    QString mItemName;
    bool mIsPlayable;
};

#endif // MEDIAITEM_H
