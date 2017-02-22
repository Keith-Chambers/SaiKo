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
    Q_INVOKABLE void nextSong(void);
    Q_INVOKABLE void prevSong(void);
    Q_INVOKABLE bool getIsPlaying(void);
    Q_INVOKABLE void setIsPlaying(bool pIsPlaying);
    Q_INVOKABLE void setPlayPosition(double pPos);
    Q_INVOKABLE double getPlayPosition(void);
    Q_INVOKABLE void togglePause(void);
    Q_INVOKABLE void stopAudio(void);
    void playMusic(void);
signals:
    void playPositionChanged(double);
    void isPlayingChanged(bool);
public slots:
    void setPlaylist(QStringList playlist);
private:
   irrklang::ISoundEngine* mEngine;
   irrklang::ISound* mCurrentSound;
   QStringList mPlaylist;
   int mPlaylistIndex;
   // Is this even used?
   //QLinkedList<irrklang::ISound*> mPlayQueue;
   SoundStopEvent mSoundStopEvent;
};

#endif // AUDIOPLAYER_H
