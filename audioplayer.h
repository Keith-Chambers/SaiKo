#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include <QObject>
#include <QDebug>
#include <QLinkedList>

// irrKlangIncludes
#include "./inc/irrKlang.h"
#include <conio.h>

#include "soundstopevent.h"

class AudioPlayer : public QObject
{
    Q_OBJECT

    Q_PROPERTY(double playPosition READ getPlayPosition WRITE setPlayPosition NOTIFY playPositionChanged)
    Q_PROPERTY(bool isPlaying READ getIsPlaying WRITE setIsPlaying NOTIFY isPlayingChanged)
public:
    explicit AudioPlayer(QObject *parent = 0);
    ~AudioPlayer();
    Q_INVOKABLE void nextSong();
    Q_INVOKABLE void prevSong();
    Q_INVOKABLE bool getIsPlaying();
    Q_INVOKABLE void setIsPlaying(bool isPlaying);
    Q_INVOKABLE void setPlayPosition(double pos);
    Q_INVOKABLE double getPlayPosition();
    Q_INVOKABLE void togglePause();
    Q_INVOKABLE void stopAudio();
    //void setPlaylist(QStringList playlist);
    void setupMusic();
    //Q_INVOKABLE boolean setSong(QString directory, QString file);
signals:
    void playPositionChanged(double);
    void isPlayingChanged(bool);
public slots:
    void setPlaylist(QStringList playlist);
private:
   irrklang::ISoundEngine* m_engine;
   irrklang::ISound* m_curr_sound;
   QStringList playlist;
   int playlistIndex;
   QLinkedList<irrklang::ISound*> m_playQueue;
   SoundStopEvent soundStopEvent;
};

#endif // AUDIOPLAYER_H
