#ifndef AUDIOPLAYLIST_H
#define AUDIOPLAYLIST_H

#include <QList>

#include <audiofile.h>

class AudioPlaylist : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int numberTracks READ getNumberTracks NOTIFY numberTracksChanged)
    Q_PROPERTY(QString name READ getName WRITE setName NOTIFY nameChanged)

public:
    AudioPlaylist(QString name);

    int getNumberTracks();
    QString getName();
    void setName(QString name);
    const QList<AudioFile>& getTracks();

    void addTrack(const AudioFile& audio);
    void removeTrack(int index);

    // TODO: Implement if needed
//    void addTrackUnique(const AudioFile& audio);

signals:
    void numberTracksChanged(int);
    void tracksChanged(QList<AudioFile>);
    void nameChanged(QString);

private:

    QString m_name;
    QList<AudioFile> m_audio_files;
};

#endif // AUDIOPLAYLIST_H
