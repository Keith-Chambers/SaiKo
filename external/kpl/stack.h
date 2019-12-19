#ifndef STACK_H
#define STACK_H

#include <stdint.h>
#include <cstdlib>
#include <cstring>
#include <assert.h>
#include <iostream>

/*
 * 0 data
 * 1 data
 * 2 data
 * 3 3
 * 4 data
 * 5 data
 * 6 2
 * 7 data
 * 8 1
 * 9 { taken = 8, left = 0 } <-- stack_ptr
 */

/* Pop
 *
 * 0 data
 * 1 data
 * 2 data
 * 3 3
 * 4 data
 * 5 data
 * 6 2
 * 7 { taken = 6, left = 2 } <-- stack_ptr
 * 8
 * 9
 */

struct DataHandleInfo
{
    uint32_t offset;
    uint32_t size;      // remove this
    uint8_t initial_padding;
    uint8_t flags;
    uint8_t handle_id;
    uint8_t buffer_id;  // You can use a pivot array for this, to indicate at what index data is being stored at next buffer
                        // Can be a bit field that just marks the mem blocks that start on a new buffer
};

struct RandomOrderMemory
{
    uint32_t size;
};

struct ChunkedString
{
    uint16_t chunk_size = 512;
};

struct SimpleMemoryStore
{
    uint32_t initial_size;
    uint32_t free_offset;
    uint16_t buffer_chunk_size;
    // uint8_t * initial_buffer; // Allocate below
    uint8_t * overflow_buffers[4]; // Allocate additional buffers on heap..
};

namespace kpl
{

    // Put at the end of allocated memory
    // Added onto the size param given at init time
    // I.e When you init with 1024 bytes, that's how much free space you have for data
    struct IndexedMemory
    {
        DataHandleInfo data_handles[1];

        uint32_t config_initial_size;
        uint32_t config_overflow_buffers_size;

        uint32_t runtime_size;

        uint32_t free_offset;

        uint8_t num_data_handles;
        /*  1 - Whether there is a free middle data handle
         *
         */
        uint8_t flags; // Change this to num_free_spaces
        uint8_t * overflow_buffers[16];
    };

    IndexedMemory * init_indexed_memory(uint32_t size, uint8_t * placement, uint32_t overflow_buffers_size)
    {
        if(placement == nullptr) {
            placement = new uint8_t[sizeof(IndexedMemory) + size];

            if(! placement)
            {
                std::cerr << "Out of memory. Terminating" << std::endl;
                exit(EXIT_FAILURE);
            }
        }

        IndexedMemory * memory_handle = reinterpret_cast<IndexedMemory *>( placement + size - sizeof(IndexedMemory) );

        memory_handle->config_initial_size = overflow_buffers_size;
        memory_handle->config_initial_size = size;
        memory_handle->runtime_size = 0;
        memory_handle->num_data_handles = 0;

        return memory_handle;
    }

    struct MemoryRecord
    {
        uint32_t size;
        uint8_t id;
        uint8_t * start;
    };

    MemoryRecord reserve_memory_block(IndexedMemory * memory_handle, uint32_t size, uint16_t alignment)
    {
        uint8_t next_handle = memory_handle->num_data_handles;

        if(next_handle == 0)
        {
            DataHandleInfo * data_handle = &memory_handle->data_handles[0];
            data_handle->size = size;
            data_handle->offset = 0;
            data_handle->buffer_id = 0;
            data_handle->flags = 0;
            data_handle->handle_id = 0;
            data_handle->initial_padding = 0; // TODO: Proper alignment

            memory_handle->runtime_size = size + data_handle->initial_padding;
            memory_handle->num_data_handles++;

            return { size, next_handle, reinterpret_cast<uint8_t *>(memory_handle) - memory_handle->config_initial_size,  };
        }

        // Already existing handles

        uint8_t buffer_index = 1;
        uint64_t runtime_size = memory_handle->runtime_size;

        if((runtime_size + size) <= memory_handle->config_initial_size)
        {
            // Fits into initial buffer
            buffer_index = 0;
        } else {

            runtime_size -= memory_handle->config_initial_size;
            uint32_t overflow_buffer_size = memory_handle->config_overflow_buffers_size;

            while(runtime_size > overflow_buffer_size) {
                buffer_index++;
                runtime_size -= overflow_buffer_size;
            }
        }

        uint32_t buffer_offset = 0;

        // If buffer index of prev data handle is different, then we are starting on a new overflow buffer & offset = 0
        if(memory_handle->data_handles[next_handle * -1].buffer_id == buffer_index)
        {
            DataHandleInfo * prev_data_handle = &memory_handle->data_handles[(next_handle - 1) * -1];
            buffer_offset = prev_data_handle->size +
                            prev_data_handle->offset +
                            prev_data_handle->initial_padding;

        }

        uint8_t * data_location;

        if(buffer_index == 0) {
            data_location = reinterpret_cast<uint8_t *>(memory_handle) - memory_handle->config_initial_size + buffer_offset;
        } else {
            data_location = memory_handle->overflow_buffers[buffer_index - 1] + buffer_offset;
        }

        DataHandleInfo * data_handle = &memory_handle->data_handles[(next_handle) * -1];
        data_handle->size = size;
        data_handle->offset = buffer_offset;
        data_handle->buffer_id = buffer_index;
        data_handle->flags = 0;
        data_handle->handle_id = next_handle;
        data_handle->initial_padding = 0; // TODO: Proper alignment

        memory_handle->runtime_size = size + data_handle->initial_padding;
        memory_handle->num_data_handles++;

        return { size, next_handle, data_location };
    }

    void free_memory_block()
    {

    }

    uint32_t data_handle_alloc_size(DataHandleInfo * data_handle)
    {
        return data_handle->size + data_handle->initial_padding;
    }

    uint64_t usage_space_left(IndexedMemory memory)
    {
        return 0;
    }

    uint64_t raw_space_left(IndexedMemory memory)
    {

    }

    struct SparceMemory
    {
        uint8_t memory_handle;

    };

    void sparce_memory_init(uint8_t * data, uint32_t size, uint8_t num_blocks)
    {

    }

    void copy(IndexedMemory * handle, uint8_t source, uint64_t offset, uint8_t * data, uint64_t length)
    {

    }

    class Buffer
    {
    public:
        Buffer(uint32_t size)
        {
            mData = static_cast<uint8_t *>(malloc(size));
            assert(mData != nullptr && "Failed to allocate kpl::Buffer");

            mSize = size;
        }

        ~Buffer()
        {
            assert(mData != nullptr && "Attempt to free null data in kpl::buffer destructor");
            free(mData);
        }

        uint8_t * mData = nullptr;
        uint32_t mSize = 0;
        uint32_t mUsed = 0;

        bool windForward(uint64_t amount)
        {
            if(mUsed + amount > mSize) {
                assert(false && "Forwarding buffer beyond end"); // TODO: Remove
                return false;
            }

            mUsed += amount;
            mData += amount;

            return true;
        }

        bool windBack(uint64_t amount)
        {
            assert(mUsed >= amount && "Attemp to wind buffer back before beginning");

            mUsed -= amount;

            return true;
        }
    };

    // TODO: Would it be faster to allocate the stack backwards, on the premise that prefetching work in ascending order?
    // You would only need to access in ascening order for write operations, and backwards for read ops.
    // Writing should not be affected by mem fetch latency since the CPU can assume the action will be successfully carried out

    struct StackSize
    {
        uint32_t space_taken;
        uint32_t space_left;
    };

    bool push(uint8_t * stack_ptr, uint8_t alignment, uint8_t * data, uint32_t data_size)
    {
        StackSize pre_size = { *reinterpret_cast<uint32_t*>(stack_ptr), *reinterpret_cast<uint32_t*>(stack_ptr + 4) };

        uint16_t padding = reinterpret_cast<uint64_t>(stack_ptr) % alignment;
        padding = alignment - padding;
        padding = (padding == alignment) ? 0 : padding;

        assert(padding < alignment && "Padding >= alignment in stack push");
        assert((reinterpret_cast<uint64_t>(stack_ptr) + padding) % alignment == 0
               && "Incorrect alignment calculated for stack push");

        if(pre_size.space_left < padding + data_size + 4)
            return false;

        memcpy(reinterpret_cast<void *>(stack_ptr + padding), reinterpret_cast<void *>(data), data_size);

        uint32_t total_size = padding + data_size;

        stack_ptr += padding + data_size + 4;

        *reinterpret_cast<uint32_t*>(stack_ptr - 4) = total_size;
        *reinterpret_cast<uint32_t*>(stack_ptr + 0) = pre_size.space_taken + total_size;
        *reinterpret_cast<uint32_t*>(stack_ptr + 8) = pre_size.space_left - (total_size);

        return true;
    }

    uint8_t * top_view(uint8_t * stack_ptr)
    {
        return reinterpret_cast<uint8_t *>(stack_ptr - *reinterpret_cast<uint32_t *>(stack_ptr - 4));
    }

    bool pop(uint8_t * stack_ptr)
    {
        StackSize pre_size = { *reinterpret_cast<uint32_t*>(stack_ptr), *reinterpret_cast<uint32_t*>(stack_ptr + 4) };

        if(pre_size.space_taken == 0)
            return false;

        uint32_t last_element_size = *reinterpret_cast<uint32_t*>(stack_ptr - 4);
        stack_ptr -= last_element_size;

        *reinterpret_cast<uint32_t*>(stack_ptr + 0) = pre_size.space_taken - last_element_size;
        *reinterpret_cast<uint32_t*>(stack_ptr + 4) = pre_size.space_left + last_element_size;

        return true;
    }

    void init_stack(uint32_t allocation_size, uint8_t * stack_start)
    {
        *reinterpret_cast<uint32_t*>(stack_start) = 0;
        *reinterpret_cast<uint32_t*>(stack_start + 4) = allocation_size - 8;
    }
}



#endif // STACK_H
