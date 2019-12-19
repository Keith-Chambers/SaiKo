#ifndef ARRAY_H
#define ARRAY_H

#include <cstdint>
#include <functional>
#include <string>

#include <kpl/chainedarray.h>

namespace kpl
{
    template <typename T>
    class array
    {
    public:
        virtual uint16_t size() const =0;
        virtual T& tRef(uint16_t index)=0;
        virtual const T& cTRef(uint16_t index) const =0;
        virtual T t(uint16_t index) const =0;
        virtual bool pushBack(const T& t)=0;
        virtual bool pushBack(T&& t)=0;
        virtual bool insertAt(const T& t, uint16_t index)=0;
        virtual bool insertAt(T&& t, uint16_t index)=0;
        virtual bool remove(uint16_t index)=0;
        virtual bool remove(uint16_t index, uint16_t numElements)=0;
        virtual bool removeEnd()=0;

        virtual ~array() {}
    };
}

#endif // ARRAY_H
