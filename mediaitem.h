#ifndef MEDIAITEM_H
#define MEDIAITEM_H

#include <QObject>
#include <QImage>
#include <QString>
#include <QDebug>
#include <QDir>

#include <iostream>

#include "textparser.h"

class MediaItem : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString itemName READ getItemName WRITE setItemName NOTIFY itemNameChanged)
    Q_PROPERTY(QString imagePath READ getImagePath WRITE setImagePath NOTIFY imagePathChanged)
    Q_PROPERTY(bool isPlayable READ getIsPlayable WRITE setIsPlayable NOTIFY isPlayableChanged)
    Q_PROPERTY(QString extension READ getExtension WRITE setExtension NOTIFY extensionChanged)
public:
    //TODO: Remove default constructor
    //explicit MediaItem(QObject *parent = 0);
    MediaItem(QString pItemPath, bool pIsPlayable, QObject *parent = nullptr);
    MediaItem(QString pItemPath, QString pImagePath, bool pIsPlayable, QObject *parent = nullptr);
    ~MediaItem();

    void applyOption(QDir directory, ParseValue option)
    {
        trimEnd(option.value);
        trimFront(option.value);

        switch(option.optionType)
        {
        case SaikOption::FRONT_COVER:
            qDebug() << "Setting from cover to " << directory.path() + "/" + QString::fromStdString(option.value);
            mImagePath = QString("file:/") + directory.path() + "/" + QString::fromStdString(option.value);
            break;
        case SaikOption::BACK_COVER:
            // TODO: Implement front and back cover
            mImagePath = QString::fromStdString(option.value);
            break;
        case SaikOption::INVALID:
            qDebug() << "Warning: Invalid Parse option reached";
            break;
        }
    }

    void applyFolderOptions(QDir directory)
    {
        if(!directory.exists(".saik")) {
            qDebug() << "Warning: No .saik file found for directory -> " << directory.path();
            return;
        }

        QFile file(directory.path() + "/.saik");

        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
            qDebug() << "Failed to open " << file.fileName();
            return;
        }

        while(!file.atEnd())
        {
            std::string line = file.readLine().toStdString();
            std::cout << ".Saik Line -> '" << line << "'" << std::endl;
            ParseValue option = parseSaikoLine(line, PARSE_OPTIONS);
            applyOption(directory, option);
        }
    }

    QString getItemName(){ return mItemName; }
    QString getImagePath(){ return mImagePath; }
    bool getIsPlayable(){ return mIsPlayable; }
    QString getExtension(){ return mExtension; }

    void setItemName(QString pItemName);
    void setImagePath(QString pDirImage);
    void setIsPlayable(bool pIsPlayable);
    void setExtension(QString pExtension);
signals:
    void itemNameChanged(QString pItemName);
    void imagePathChanged(QString pDirImage);
    void isPlayableChanged(bool pIsPlayable);
    void extensionChanged(QString pExtension);

public slots:

private:
    QString mImagePath;
    QString mItemName;
    bool mIsPlayable; // Can be removed. Use extension to determine this
    QString mExtension;

    QDir mLocation;
    bool mIsFolder;
};

#endif // MEDIAITEM_H
