#ifndef AUDIOFILE_H
#define AUDIOFILE_H

#include <QObject>
#include <QString>
#include <QImage>

class AudioFile : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString title READ getName NOTIFY nameChanged)
    Q_PROPERTY(QString artist READ getArtist NOTIFY artistChanged)
    Q_PROPERTY(QImage art READ getArt NOTIFY artChanged)
    Q_PROPERTY(bool hasArt READ getHasArt NOTIFY hasArtChanged)

public:
    AudioFile(QString pName, QString pArtist);
    AudioFile(QString pName, QString pArtist, QImage* pArt);

    AudioFile(AudioFile&& audioFile);

    QString getName();
    QString getArtist();
    QImage getArt();
    bool getHasArt();

signals:
    void nameChanged(QString);
    void artistChanged(QString);
    void artChanged(QImage);
    void hasArtChanged(bool);

private:
    QString mName;
    QString mArtist;
    QImage* mArt;
};

#endif // AUDIOFILE_H
