#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include <QObject>
#include <QDebug>
#include <QLinkedList>
#include <QTimer>

#include <QDir>
#include <QQmlListProperty>

#include <irrKlang/irrKlang.h>

#include <kpl/filesystem.h>

#include "soundstopevent.h"
#include "textparser.h"
#include "audiofile.h"

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
    Q_PROPERTY(bool isPlaying READ getIsPlaying WRITE setPlayState NOTIFY isPlayingChanged)
    Q_PROPERTY(double volume READ getVolume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(QString playPositionTime READ getPlayPositionTime NOTIFY playPositionTimeChanged)

//    Q_PROPERTY(QQmlListProperty<AudioFrameSlice> currentAudioFrame READ getCurrentAudioFrame NOTIFY currentAudioFrameChanged)

public:
    explicit AudioPlayer(QObject *parent = nullptr);
    ~AudioPlayer();


    QString getPlayPositionTime();

    Q_INVOKABLE bool getIsPlaying(void);
    Q_INVOKABLE double getPlayPosition(void);
    void updateAudioPosition();
    double getVolume();
    void setVolume(double volume);
signals:
    void playPositionChanged(double);
    void audioChanged(QString name);
    void audioCompleted();
    void isPlayingChanged(bool);
    void volumeChanged(double);
    void playPositionTimeChanged(QString);
public slots:
    Q_INVOKABLE void playAudio(AudioFile audio);
    Q_INVOKABLE void pauseAudio();
    Q_INVOKABLE void resumeAudio();
    Q_INVOKABLE void togglePause();
    Q_INVOKABLE void setPlayPosition(double position);
    Q_INVOKABLE void setPlayState(bool is_playing);
private:
    irrklang::ISoundEngine*             m_irrklang_engine;
    irrklang::ISound*                   m_irrklang_sound;
    SoundStopEvent                      m_sound_stop_event;

    double                              m_volume;
    double                              m_saved_audio_position;
    QTimer                              m_update_audio_position_timer;
    std::optional<kfs::FileIdentifier>  m_current_audio;
};

#endif // AUDIOPLAYER_H
