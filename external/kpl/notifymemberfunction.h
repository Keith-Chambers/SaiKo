#ifndef NOTIFYMEMBERFUNCTION_H
#define NOTIFYMEMBERFUNCTION_H

#include "kpl/eventnotifier.h"

namespace kpl
{

    template <class T>
    class NotifyMemberFunction : public EventNotifier
    {
    public:
        NotifyMemberFunction(T *pContextObject, void (T::*pMemberFunctor)());
        ~NotifyMemberFunction();
        virtual void notify();
    private:
        void (T::*mMemberFunctor)(void);
        T *mContextObject;
    };

    template <class T>
    NotifyMemberFunction<T>::NotifyMemberFunction(T *pContextObject, void (T::*pMemberFunctor)())
    {
        mContextObject = pContextObject; 
        mMemberFunctor = pMemberFunctor;
    }

    template <class T>
    NotifyMemberFunction<T>::~NotifyMemberFunction()
    {
        
    }

    template <class T>
    void NotifyMemberFunction<T>::notify()
    {
        (*mContextObject.*mMemberFunctor)();
    }
}

#endif // NOTIFYMEMBERFUNCTION_H
