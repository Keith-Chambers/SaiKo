#ifndef EVENTNOTIFIER_H
#define EVENTNOTIFIER_H

namespace kpl
{

    class EventNotifier
    {
    public:
        virtual void notify() = 0;
    };
}

#endif // EVENTNOTIFIER_H
