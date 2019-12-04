#include "soundstopevent.h"
#include "audioplayer.h"

SoundStopEvent::SoundStopEvent()
{
    qDebug() << "soundStopEvent constructor called";
}

void SoundStopEvent::OnSoundStopped(irrklang::ISound *pSound, irrklang::E_STOP_EVENT_CAUSE pReason, void *pUserData)
{
    if(pReason != irrklang::E_STOP_EVENT_CAUSE::ESEC_SOUND_FINISHED_PLAYING ) {
        return;
    }
//    ((AudioPlayer*)pUserData)->nextSong();
    qDebug() << "onSoundStopped event complete";
}
