#ifndef SOUNDSTOPEVENT_H
#define SOUNDSTOPEVENT_H

#include <QDebug>

#include "./inc/irrKlang.h"

class SoundStopEvent : irrklang::ISoundStopEventReceiver
{
public:
    SoundStopEvent();
    void OnSoundStopped(irrklang::ISound *pSound, irrklang::E_STOP_EVENT_CAUSE pReason, void * pUserData);
private:

};

#endif // SOUNDSTOPEVENT_H
