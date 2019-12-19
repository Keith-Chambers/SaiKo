#ifndef MEMBERFUNCTOR_H
#define MEMBERFUNCTOR_H

#include "kpl/datasource.h"

namespace kpl
{

    template <class T>
    class MemberFunctor : public DataSource
    {
    public:
        MemberFunctor(T *pContextObject, void *(T::*pMemberFunctor)());
        ~MemberFunctor();
        virtual void * get();
    private:
        void *(T::*mMemberFunctor)(void);
        T *mContextObject;
    };

    template <class T>
    MemberFunctor<T>::MemberFunctor(T *pContextObject, void *(T::*pMemberFunctor)())
    {
        mContextObject = pContextObject; 
        mMemberFunctor = pMemberFunctor;
    }


    template <class T>
    void * MemberFunctor<T>::get()
    {
        return (*mContextObject.*mMemberFunctor)();
    }


    template <class T>
    MemberFunctor<T>::~MemberFunctor()
    {
        
    }

}

#endif // MEMBERFUNCTOR_H
