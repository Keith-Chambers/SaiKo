#ifndef SOUNDSTOPEVENT_H
#define SOUNDSTOPEVENT_H

#include <QDebug>

#include "./inc/irrKlang.h"

class SoundStopEvent : irrklang::ISoundStopEventReceiver
{
public:
    SoundStopEvent();
    void OnSoundStopped(irrklang::ISound *sound, irrklang::E_STOP_EVENT_CAUSE reason, void * userData);
    //~SoundStopEvent(){}
};

#endif // SOUNDSTOPEVENT_H
