#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include <QObject>
#include <QDebug>
#include <QLinkedList>
#include <QTimer>

// irrKlangIncludes
#include "./inc/irrKlang.h"

#include "soundstopevent.h"
#include "audiofile.h"

class AudioPlayer : public QObject
{
    Q_OBJECT

    Q_PROPERTY(double playPosition READ getPlayPosition WRITE setPlayPosition NOTIFY playPositionChanged)
    Q_PROPERTY(bool isPlaying READ getIsPlaying WRITE setIsPlaying NOTIFY isPlayingChanged)
//    Q_PROPERTY(AudioFile currentAudio READ getCurrentAudio NOTIFY currentAudioChanged)
public:
    explicit AudioPlayer(QObject *parent = nullptr);
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
    void updateAudioPosition();
    void setCurrentAudio(QString path);
signals:
    void playPositionChanged(double);
    void isPlayingChanged(bool);
    void songChanged();
public slots:
    void setPlaylist(QStringList playlist);
private:
   irrklang::ISoundEngine* mEngine;
   irrklang::ISound* mCurrentSound;
   QStringList mPlaylist;
   int mPlaylistIndex;
   SoundStopEvent mSoundStopEvent;
   QTimer* mPositionTimer;
   AudioFile mCurrentAudio;
};

#endif // AUDIOPLAYER_H
