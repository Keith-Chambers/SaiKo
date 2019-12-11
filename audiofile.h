#ifndef AUDIOFILE_H
#define AUDIOFILE_H

#include <QObject>
#include <QString>

class AudioFile : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString title READ getTitle NOTIFY titleChanged)
    Q_PROPERTY(QString artist READ getArtist NOTIFY artistChanged)
    Q_PROPERTY(QString artPath READ getArtPath NOTIFY artPathChanged)
    Q_PROPERTY(bool hasArt READ getHasArt NOTIFY hasArtChanged)

public:
    AudioFile(QString pName, QString pArtist);
    AudioFile(QString pName, QString pArtist, QString pArtPath);

    AudioFile(AudioFile&& audioFile);

    void setTitle(QString title);
    void setArtist(QString artist);
    void setArtPath(QString artPath);

    QString getTitle();
    QString getArtist();
    QString getArtPath();
    bool getHasArt();

signals:
    void titleChanged(QString);
    void artistChanged(QString);
    void artPathChanged(QString);
    void hasArtChanged(bool);

private:
    QString mTitle;
    QString mArtist;
    QString mArtPath;
};

#endif // AUDIOFILE_H
