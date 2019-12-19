#ifndef CHAINEDARRAY_H
#define CHAINEDARRAY_H

// #define CHAINED_ARRAY_DEBUG

#ifdef CHAINED_ARRAY_DEBUG

#include <fmt/format.h>
#include <assert.h>
#include <cstdio>
#include <functional>

#endif

#include <cstdlib>
#include <cstdint>
#include <cstring>

#include <kpl/array.h>

#include <kpl/util.h>
#include <kpl/pair.h>

/*  Use case: When you are reasonably sure that you want to store a small amount of elements
 *  but need to ability to store far more to be safe */
namespace kpl{

    template <typename T, std::size_t STACK_PRE_ALLOC, std::size_t DYNA_BUFFER_SIZE, std::size_t NUM_DYNA_BUFFERS>
    class ChainedArray : public kpl::array<T>
    {
    private:
        uint16_t mNumElements;
        uint8_t mDynaAllocatedMemoryBlocks;
        uint8_t * mDynaMemBuffer[NUM_DYNA_BUFFERS];
        uint8_t mStackBuffer[sizeof(T) * STACK_PRE_ALLOC];
        const uint32_t MAX_CAPACITY = STACK_PRE_ALLOC + (DYNA_BUFFER_SIZE * NUM_DYNA_BUFFERS);

    public:
        ChainedArray()
            : mNumElements { 0 },
              mDynaAllocatedMemoryBlocks {0}
        {
            for(uint8_t i = 0; i < NUM_DYNA_BUFFERS; i++)
                mDynaMemBuffer[i] = nullptr;

        }

        /* Getters */
        T t(uint16_t index) const override
        {
            uint8_t * location = const_cast<uint8_t*>(addrForElement(index));
            return *(reinterpret_cast<T*>(location));
        }

        T& tRef(uint16_t index) override
        {
            uint8_t * location = const_cast<uint8_t*>(addrForElement(index));
            return *(reinterpret_cast<T*>(location));
        }

        const T& cTRef(uint16_t index) const override
        {
            const uint8_t * location = addrForElement(index);
            return *(reinterpret_cast<const T*>(location));
        }

        uint16_t size() const
        {
            return mNumElements;
        }

        /* Modifiers */
        bool removeEnd() override
        {
            if(mNumElements <= 0)
                return false;

            mNumElements--;
            return true;
        }

        bool remove(uint16_t index) override
        {
            if(index >= mNumElements)
                return false;

            // Left shift will overwrite index
            leftShiftData(index, 1);
            mNumElements--;

            return true;
        }

        bool remove(uint16_t index, uint16_t numElements) override
        {
            if(index + numElements >= mNumElements)
                return false;

            leftShiftData(index, numElements);
            mNumElements -= numElements;

            return true;
        }

        bool pushBack(const T& t) override
        {
            if(mNumElements == MAX_CAPACITY)
            {
                #ifdef CHAINED_ARRAY_DEBUG
                    assert(false && "Err: Insertion at max value");
                #endif
                return false;
            }

            uint8_t * placement = prepareBackForPlacement();

            if(! placement)
            {
                #ifdef CHAINED_ARRAY_DEBUG
                    assert(false && "Failed to push value element");
                #endif
                return false;
            }

            new (  reinterpret_cast<void*>(placement) ) T( t );
            mNumElements++;

        #ifdef CHAINED_ARRAY_DEBUG
            fmt::print("Element added. Size -> {}\n", mNumElements);
        #endif

            return true;
        }

        bool pushBack(T&& t) override
        {

            if(mNumElements == MAX_CAPACITY)
            {
                #ifdef CHAINED_ARRAY_DEBUG
                    assert(false && "Err: Insertion at max value");
                #endif
                return false;
            }

            uint8_t * placement = prepareBackForPlacement();

            if(! placement)
            {
                #ifdef CHAINED_ARRAY_DEBUG
                    assert(false && "Failed to push r-value element");
                #endif
                return false;
            }

            new (  reinterpret_cast<void*>(placement) ) T( std::move(t) );
            mNumElements++;

        #ifdef CHAINED_ARRAY_DEBUG
            fmt::print("Element added. Size -> {}\n", mNumElements);
        #endif

            return true;
        }

        bool insertAt(const T& t, uint16_t index) override
        {

            if(mNumElements == MAX_CAPACITY)
            {
                #ifdef CHAINED_ARRAY_DEBUG
                    assert(false && "Err: Insertion at max value");
                #endif
                return false;
            }

            uint8_t * placement = prepareIndexForPlacement(index);

            if(! placement)
            {
                #ifdef CHAINED_ARRAY_DEBUG
                    assert(false && "Failed to insert value element by value");
                #endif
                return false;
            }

            // Place new element in the cleared space
            new (  placement ) T( t );
            mNumElements++;

            return true;
        }

        bool insertAt(T&& t, uint16_t index) override
        {

            if(mNumElements == MAX_CAPACITY)
            {
                #ifdef CHAINED_ARRAY_DEBUG
                    assert(false && "Err: Insertion at max value");
                #endif
                return false;
            }

            uint8_t * placement = prepareIndexForPlacement(index);

            if(! placement)
            {
            #ifdef CHAINED_ARRAY_DEBUG
                fmt::print("Error: Failed to get placement address for insertAt op\n");
                dumpState();
                assert(false);
            #endif
                return false;
            }

            // Place new element in the cleared space
            new (  placement ) T( std::move(t) );
            mNumElements++;

            return true;
        }

        T * setNext()
        {
            T * result = reinterpret_cast<T *>( prepareBackForPlacement() );
            mNumElements++;
            return result;
        }

        /*  Contructs a type T at the memory address calculate to placement using T's constructor values
         *  E.G Instead of push_back( t ), you do emplace_back( a, b, c ) where type T has a constructor of T(a, b, c */
        template <typename... Ts>
        bool emplaceBack(Ts&&... args)
        {

            if(mNumElements == MAX_CAPACITY)
            {
                #ifdef CHAINED_ARRAY_DEBUG
                    assert(false && "Err: Insertion at max value");
                #endif
                return false;
            }

            uint8_t * placement = (mNumElements < STACK_PRE_ALLOC) ? (&mStackBuffer[mNumElements]) : prepareBackForPlacement();

            if(! placement)
            {
            #ifdef CHAINED_ARRAY_DEBUG
                fmt::print("Error: Failed to get placement address for emplaceBack op\n");
                dumpState();
                assert(false);
            #endif
                return false;
            }

            new (  reinterpret_cast<void*>(placement) ) T( static_cast<Ts&&>(args)... );
            mNumElements++;

        #ifdef CHAINED_ARRAY_DEBUG
            fmt::print("Element added. Size -> {}\n", mNumElements);
        #endif

            return true;
        }

    #ifdef CHAINED_ARRAY_DEBUG
        void printContents( std::function<void(const T)> printFunc)
        {
            for(uint16_t i = 0; i < mNumElements; i++)
                printFunc( cTRef(i) );
        }
    #endif

        template <typename... Ts>
        bool emplaceAt(Ts&&... args, uint16_t index)
        {

            if(mNumElements == MAX_CAPACITY)
            {
                #ifdef CHAINED_ARRAY_DEBUG
                    assert(false && "Err: Insertion at max value");
                #endif
                return false;
            }

            uint8_t * placement = prepareIndexForPlacement(index);

            if(! placement)
            {
                #ifdef CHAINED_ARRAY_DEBUG
                    assert(false && "Failed to push value element by value");
                #endif
                return false;
            }

            new (  reinterpret_cast<void*>(placement) ) T( static_cast<Ts&&>(args)... );
            mNumElements++;

            return true;
        }

        kpl::array<T> * asArray()
        {
            return dynamic_cast<kpl::array<T> *>(this);
        }

    private:
        // Assume valid dynaBufferIndex
        bool ensureBufferAllocated(uint16_t dynaBufferIndex)
        {
            if(dynaBufferIndex == 0)
                return true;

            if(mDynaMemBuffer[dynaBufferIndex - 1] == nullptr)
                mDynaMemBuffer[dynaBufferIndex - 1] = reinterpret_cast<uint8_t*>( malloc( sizeof(T) * DYNA_BUFFER_SIZE ) );

            #ifdef CHAINED_ARRAY_DEBUG
                fmt::print(stderr, "Dynamic Buffer # {} allocated\n", dynaBufferIndex - 1);
            #endif

            assert( mDynaMemBuffer[dynaBufferIndex - 1] );

            return true;
        }

        uint16_t bufferIndexFor(uint16_t index) const
        {
            if(index < STACK_PRE_ALLOC)
                return index;
            
            uint16_t result = 1;
            index -= STACK_PRE_ALLOC;
            
            while(index >= DYNA_BUFFER_SIZE)
            {
                result++;
                index -= DYNA_BUFFER_SIZE;
            }
            
            return result;
        }

        const uint8_t * addrForElement(uint16_t index) const
        {
            // Make sure requested index is in range
            if( STACK_PRE_ALLOC + (DYNA_BUFFER_SIZE * NUM_DYNA_BUFFERS) <= index )
            {
                #ifdef CHAINED_ARRAY_DEBUG
                fmt::print(stderr, "Error: Requested address exceeds max allocated memory in ChainedArray\n"
                           "Requested Index: {}\n"
                           "Preallocated Stack Memory: {}\n"
                           "Dynamic Memory Block Size: {}\n"
                           "Max Number Dyanamic Memory Blocks: {}\n", index, STACK_PRE_ALLOC, DYNA_BUFFER_SIZE, NUM_DYNA_BUFFERS);

                exit(EXIT_FAILURE);
                #endif

                return nullptr;
            }

            
            kpl::Pair<uint16_t, uint16_t> absoluteIndexPair = translateIndex(index);
            
        #ifdef CHAINED_ARRAY_DEBUG
            fmt::print("Absolute Index Pair for Raw Index of {} -> {}:{}\n", index, absoluteIndexPair.getT1CRef(), absoluteIndexPair.getT2CRef());
        #endif

            // Get address from stack memory
            if(absoluteIndexPair.getT1CRef() == 0)
                return reinterpret_cast<const uint8_t *>(& (mStackBuffer[index * sizeof(T)]));

            // Since index has been localised to a memory block, it should not exceed the size of one
            if(absoluteIndexPair.getT2CRef() > DYNA_BUFFER_SIZE)
            {
            #ifdef CHAINED_ARRAY_DEBUG

                fmt::print(stderr, "Error: index({}) > DYNA_BUFFER_SIZE({}) assert failed in ChainedArray::addrForElement({})\n", absoluteIndexPair.getT2CRef(), DYNA_BUFFER_SIZE, absoluteIndexPair.getT2CRef());
                assert(absoluteIndexPair.getT2CRef() <= DYNA_BUFFER_SIZE);
            #endif

                return nullptr;
            }

            // Get beginning of correct memory block and move index * sizeof(T) bytes forward
            return reinterpret_cast<const uint8_t *>(mDynaMemBuffer[absoluteIndexPair.getT1CRef() - 1] + (absoluteIndexPair.getT2CRef() * sizeof(T)));
        }

        /* BufferIndex, RelativeIndex */
        kpl::Pair<uint16_t, uint16_t> translateIndex(uint16_t index) const
        {
            if(index < STACK_PRE_ALLOC)
                return kpl::Pair<uint16_t, uint16_t>(0, index);
            
            uint16_t result = 1;
            index -= STACK_PRE_ALLOC;
            
            while(index >= DYNA_BUFFER_SIZE)
            {
                result++;
                index -= DYNA_BUFFER_SIZE;
            }
            
            return kpl::Pair<uint16_t, uint16_t>(result, index);
        }

        T* dataPtrAt(kpl::Pair<uint16_t, uint16_t> indexPair)
        {
            if(indexPair.getT1CRef() == 0)
            {
                return reinterpret_cast<T*>(& mStackBuffer[indexPair.getT2CRef() * sizeof(T)]);
            } else
            {
                return reinterpret_cast<T*>(mDynaMemBuffer[indexPair.getT1CRef() - 1] + (indexPair.getT2CRef() * sizeof(T)));
            }
        }
        
    #ifdef CHAINED_ARRAY_DEBUG
        void dumpState()
        {
            fmt::print("Dumping chained buffer state\n\n");
            
            fmt::print("Stack allocated space: {}\n", STACK_PRE_ALLOC);
            fmt::print("Dynamic buffer size: {}\n", DYNA_BUFFER_SIZE);
            fmt::print("Number of Dynamic Buffers: {}\n", NUM_DYNA_BUFFERS);
            fmt::print("Size: {}\n", mNumElements);
            fmt::print("Tail buffer index: {}\n\n", bufferIndexFor(mNumElements));
        }
    #endif

        /* Shifts from the end until indexTo, indexTo is overridden */
        void leftShiftData(uint16_t indexTo, uint16_t shiftSize)
        {
            if(indexTo >= mNumElements)
            {
                
            #ifdef CHAINED_ARRAY_DEBUG
                fmt::print("Err: indexTo in leftShiftData is greater than total number of elements in container\n");
                fmt::print("IndexTo: {}, mNumberElements: {}\n", indexTo, mNumElements);
                assert(false);
            #endif
                
                return;
            }

            uint16_t currentIndex = indexTo + shiftSize;
            uint16_t shiftToIndex = indexTo;

            while(currentIndex < mNumElements)
            {
                kpl::Pair<uint16_t, uint16_t> srcIndexPair = translateIndex(currentIndex);
                kpl::Pair<uint16_t, uint16_t> dstIndexPair = translateIndex(shiftToIndex);

                // *dataPtrAt(dstIndexPair) = *dataPtrAt(srcIndexPair);
                std::memcpy(dataPtrAt(dstIndexPair), dataPtrAt(srcIndexPair), sizeof(T));

                shiftToIndex++;
                currentIndex++;
            }
        }

        /* TODO: Make more efficient */
        void rightShiftData(uint16_t indexFrom, uint16_t shiftSize)
        {

            // Start at end of array and work towards in shift point
            uint16_t shiftPointIndex = mNumElements - 1;
        #ifdef CHAINED_ARRAY_DEBUG
            kpl::Pair<uint16_t, uint16_t> testIndexPair = translateIndex(shiftPointIndex);
            fmt::print("Shift point: raw -> {}, bufferIndex -> {}, localIndex -> {}\n", shiftPointIndex, testIndexPair.getT1CRef(), testIndexPair.getT2CRef());
        #endif
            
            while( true )
            {
                kpl::Pair<uint16_t, uint16_t> srcIndexPair = translateIndex(shiftPointIndex);
                kpl::Pair<uint16_t, uint16_t> dstIndexPair = translateIndex(shiftPointIndex + shiftSize);
                
            #ifdef CHAINED_ARRAY_DEBUG
                fmt::print("Moving data from index {}:{} to {}:{}\n", srcIndexPair.getT1CRef(), srcIndexPair.getT2CRef(), dstIndexPair.getT1CRef(), dstIndexPair.getT2CRef());
            #endif

                std::memcpy(dataPtrAt(dstIndexPair), dataPtrAt(srcIndexPair), sizeof(T));
                
                if(shiftPointIndex-- == indexFrom)
                    break;
            }
            
            #ifdef CHAINED_ARRAY_DEBUG
                fmt::print("Data shift completed\n");
            #endif
        }

        uint8_t * prepareBackForPlacement()
        {
            // Fast path, stack only
            if(mNumElements < STACK_PRE_ALLOC)
                return &mStackBuffer[ mNumElements * sizeof(T) ];

            /* Slow path that needs to take dynamic buffers into account */

            // T1: bufferIndex, T2: Relative index
            kpl::Pair<uint16_t, uint16_t> fullIndex = translateIndex(mNumElements);

            if(! ensureBufferAllocated(fullIndex.getT1Ref()))
            {
            #ifdef CHAINED_ARRAY_DEBUG
                fmt::print("Error: Failed to allocate DYNAMIC buffer # {} for pushBack op\n", fullIndex.getT1CRef() - 1);
                dumpState();
                assert(false);
            #endif

                return nullptr;
            }

            return mDynaMemBuffer[fullIndex.getT1CRef() - 1] + ( sizeof(T) * fullIndex.getT2CRef() );
        }

        uint8_t * prepareIndexForPlacement(uint16_t index, uint16_t shiftSize = 1)
        {

            if(! ensureBufferAllocated( bufferIndexFor( mNumElements + shiftSize - 1 ) ))
            {
            #ifdef CHAINED_ARRAY_DEBUG
                fmt::print("Error: Failed to allocate buffer index # {}\n", bufferIndexFor( mNumElements + shiftSize - 1 ));
                dumpState();
                assert(false);
            #endif

                return nullptr;
            }

            // Make space for new element
            rightShiftData(index, shiftSize);

            return const_cast<uint8_t*>(addrForElement(index));
        }
    };
}

#endif // CHAINEDARRAY_H
