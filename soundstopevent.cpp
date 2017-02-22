#include "soundstopevent.h"
#include "audioplayer.h"

SoundStopEvent::SoundStopEvent()
{
    qDebug() << "soundStopEvent constructor called";
    //mAudioPlayer = pAudioPlayer;
}

void SoundStopEvent::OnSoundStopped(irrklang::ISound *sound, irrklang::E_STOP_EVENT_CAUSE reason, void * userData)
{
    if(reason != irrklang::E_STOP_EVENT_CAUSE::ESEC_SOUND_FINISHED_PLAYING )
        return;
    ((AudioPlayer*)userData)->nextSong();
    qDebug() << "onSoundStopped event called";
}
