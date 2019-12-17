#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include <QObject>
#include <QDebug>
#include <QLinkedList>
#include <QTimer>

// TODO: Part of a hack, remove
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QQmlListProperty>

#include <fileref.h>
#include <tag.h>
#include <mpeg/id3v2/id3v2tag.h>
#include <mpeg/id3v2/id3v2header.h>
#include <mpeg/id3v2/id3v2frame.h>
#include <mpeg/mpegfile.h>
#include <mpeg/id3v2/frames/attachedpictureframe.h>

// irrKlangIncludes
#include <irrKlang/irrKlang.h>

#include "soundstopevent.h"
#include "audiofile.h"
#include "textparser.h"

//QColor colorLerp(QColor from, QColor to, double lerpValue)
//{
//    int rDiff = from.red() - to.red();
//    int gDiff = from.green() - to.green();
//    int bDiff = from.blue() - to.blue();

//    return QColor(from.red() + (rDiff * lerpValue), from.green() + (gDiff * lerpValue), from.blue() + (bDiff * lerpValue));
//}

//class AudioFrameSlice : public QObject
//{
//    Q_OBJECT
//    Q_PROPERTY(QColor color READ getColor NOTIFY colorChanged)
//    Q_PROPERTY(double value READ getValue NOTIFY valueChanged)
//public:

//    AudioFrameSlice(double value)
//        : mValue {value} {}

//    double getValue(){ return mValue; }
//    QColor getColor(){ return colorLerp(QColor("white"), QColor("red"), mValue); }

//    void setValue(double value) { mValue = value; }

//signals:
//    void colorChanged(QColor);
//    void valueChanged(double);

//private:
//    double mValue;
//};

class AudioPlayer : public QObject
{
    Q_OBJECT

    Q_PROPERTY(double playPosition READ getPlayPosition WRITE setPlayPosition NOTIFY playPositionChanged)
    Q_PROPERTY(bool isPlaying READ getIsPlaying WRITE setIsPlaying NOTIFY isPlayingChanged)
    Q_PROPERTY(AudioFile * currentAudio READ getCurrentAudio NOTIFY songChanged)
    Q_PROPERTY(int currentSongIndex READ getCurrentSongIndex NOTIFY currentSongIndexChanged)
//    Q_PROPERTY(QQmlListProperty<AudioFrameSlice> currentAudioFrame READ getCurrentAudioFrame NOTIFY currentAudioFrameChanged)

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
    AudioFile* getCurrentAudio();

    int getCurrentSongIndex();
signals:
    void playPositionChanged(double);
    void isPlayingChanged(bool);
    void songChanged();
    void currentSongIndexChanged(int);
public slots:
    void setPlaylist(QStringList playlist);
private:
    static QString getAudioTitle(QFile audioFile);
    static QString getAudioArtist(QFile audioFile);
    static QString getAudioAlbum(QFile audioFile);
    static void getAudioArt(QFile audioFile);
    static QString loadAlbumArtToFileIfExists(QString filePath, QString successPath, QString failurePath);

    irrklang::ISoundEngine* mEngine;
    irrklang::ISound* mCurrentSound;
    QStringList mPlaylist;
    int mPlaylistIndex;
    SoundStopEvent mSoundStopEvent;
    QTimer* mPositionTimer;
    AudioFile mCurrentAudio;
};

#endif // AUDIOPLAYER_H
