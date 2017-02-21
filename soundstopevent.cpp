#include "soundstopevent.h"

SoundStopEvent::SoundStopEvent()
{
    qDebug() << "soundStopEvent constructor called";
}

void SoundStopEvent::OnSoundStopped(irrklang::ISound *sound, irrklang::E_STOP_EVENT_CAUSE reason, void * userData)
{
    qDebug() << "onSoundStopped event called";
}
