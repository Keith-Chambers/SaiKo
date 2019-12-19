#ifndef LINKEDBUFFERARRAY_H
#define LINKEDBUFFERARRAY_H

#include <cstdlib>
#include <kpl/array.h>
#include <kpl/chainedarray.h>

#include <functional>
#include <utility>

/*  This was intended to expose an interface of kpl::ChainedArray that allowed one to drop all template arguements except T
 *  The auther fought with the beast that is C++ for many days and nights, and in the end all that remained was this file, and a broken man.
 *
 *  TLDR; Don't use this.
 */

namespace kpl
{

    template <typename T>
    class LinkedBufferArray
    {
    public:

        template <uint16_t StackSize, uint16_t HeapBufferSize, uint8_t NumHeapBuffers>
        static LinkedBufferArray make(uint8_t * placement)
        {
            new ( reinterpret_cast<void *>(placement) )( kpl::ChainedArray<T, StackSize, HeapBufferSize, NumHeapBuffers> );

            std::function<uint16_t(void)> sizeF = [placement](void) -> uint16_t {
                return reinterpret_cast< kpl::ChainedArray<T, StackSize, HeapBufferSize, NumHeapBuffers> *>( placement )->size();
            };

            std::function<T&(uint16_t)> tRefF = [placement](uint16_t index) -> T& {
                return reinterpret_cast<kpl::ChainedArray<T, StackSize, HeapBufferSize, NumHeapBuffers> *>( placement )->tRef(index);
            };

            std::function<const T&(uint16_t)> cTRefF = [placement](uint16_t index) -> const T& {
                return reinterpret_cast<kpl::ChainedArray<T, StackSize, HeapBufferSize, NumHeapBuffers> *>( placement )->cTRef(index);
            };

            std::function<T(uint16_t)> tF = [placement](uint16_t index) -> T {
                return reinterpret_cast<kpl::ChainedArray<T, StackSize, HeapBufferSize, NumHeapBuffers> *>( placement )->t(index);
            };

            std::function<void(const T& t)> pushBackF = [placement](const T& t) -> void {
                reinterpret_cast<kpl::ChainedArray<T, StackSize, HeapBufferSize, NumHeapBuffers> *>( placement )->pushBack(t);
            };

            std::function<bool(const T& t, uint16_t index)> insertAtF = [placement](const T& t, uint16_t index) -> bool {
                return reinterpret_cast<kpl::ChainedArray<T, StackSize, HeapBufferSize, NumHeapBuffers> *>( placement )->insertAt(t, index);
            };

            std::function<bool(uint16_t index)> removeF = [placement](uint16_t index) -> bool {
                return reinterpret_cast<kpl::ChainedArray<T, StackSize, HeapBufferSize, NumHeapBuffers> *>( placement )->removeAt(index);
            };

            std::function<bool(void)> removeEndF = [placement]() -> bool {
                return reinterpret_cast<kpl::ChainedArray<T, StackSize, HeapBufferSize, NumHeapBuffers> *>( placement )->removeEnd();
            };

            return LinkedBufferArray(   reinterpret_cast<void *>(placement),
                                        StackSize,
                                        HeapBufferSize,
                                        NumHeapBuffers,
                                        std::move(sizeF),
                                        std::move(tRefF),
                                        std::move(cTRefF),
                                        std::move(tF),
                                        std::move(pushBackF),
                                        std::move(insertAtF),
                                        std::move(removeF),
                                        std::move(removeEndF)
                                        );
        }

        LinkedBufferArray(LinkedBufferArray&& linkedBufferArray)
            : LinkedBufferArray {   const_cast<void *>( linkedBufferArray.implementationData() ),
                                    linkedBufferArray.mStackSize,
                                    linkedBufferArray.mHeapBufferSize,
                                    linkedBufferArray.mNumHeapBuffers,
                                    std::move(linkedBufferArray.size),
                                    std::move(linkedBufferArray.tRef),
                                    std::move(linkedBufferArray.cTRef),
                                    std::move(linkedBufferArray.t),
                                    std::move(linkedBufferArray.pushBack),
                                    std::move(linkedBufferArray.insertAt),
                                    std::move(linkedBufferArray.remove),
                                    std::move(linkedBufferArray.removeEnd) }
        {

        }

        std::function<uint16_t(void)> size;
        std::function<T&(uint16_t)> tRef;
        std::function<const T&(uint16_t)> cTRef;
        std::function<T(uint16_t)> t;
        std::function<void(const T& t)> pushBack;
        std::function<bool(const T& t, uint16_t index)> insertAt;
        std::function<bool(uint16_t index)> remove;
        std::function<bool(void)> removeEnd;


        const uint16_t mStackSize;
        const uint16_t mHeapBufferSize;
        const uint8_t mNumHeapBuffers;

        const void * implementationData()
        {
            return mImplementationData;
        }

    private:
        LinkedBufferArray(void * implementationData,
                          uint16_t stackSize,
                          uint16_t heapBufferSize,
                          uint8_t numHeapBuffers,
                          std::function<uint16_t(void)>&& sizeF,
                          std::function<T&(uint16_t)>&& tRefF,
                          std::function<const T&(uint16_t)>&& cTRefF,
                          std::function<T(uint16_t)>&& tF,
                          std::function<void(const T& t)>&& pushBackF,
                          std::function<bool(const T& t, uint16_t index)>&& insertAtF,
                          std::function<bool(uint16_t index)>&& removeF,
                          std::function<bool(void)>&& removeEndF
             )
            :   mStackSize { stackSize },
                mHeapBufferSize { heapBufferSize },
                mNumHeapBuffers { numHeapBuffers },
                mImplementationData { implementationData },
                size { sizeF },
                tRef { tRefF },
                cTRef { cTRefF },
                t { tF },
                pushBack { pushBackF },
                insertAt { insertAtF },
                remove { removeF },
                removeEnd { removeEndF }
        {  }

        void * mImplementationData;

    };

}

#endif // LINKEDBUFFERARRAY_H
