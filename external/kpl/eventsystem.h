#ifndef EVENTSYSTEM_H
#define EVENTSYSTEM_H

#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <stdlib.h>

#include "kpl/eventnotifier.h"

namespace kpl
{

    class EventSystem
    {
    public:
        
        ~EventSystem();
        static EventSystem *getInstance();
        
        bool registerEvent(std::string pName);
        bool subscribeForEvent(std::string pName, EventNotifier * pNotifyMemberFunction);
        bool triggerEvent(std::string pName);
    private:
        static EventSystem *mInstance;
        EventSystem();
        std::unordered_map<std::string, std::vector<EventNotifier*>* > mEventBindings;
    };
}

#endif // EVENTSYSTEM_H
