#ifndef STRING_H
#define STRING_H

//#include <iostream>
//#include <stdint.h>
//#include <cstdio>
//#include <limits>
//#include <cstring>
//#include <assert.h>

#include <utility>

#include <kpl/core.h>
#include <kpl/algorithm.h>

namespace kpl {

    kpl::StringView forwardUntilView(kpl::StringView string, const kpl::Char sentinal);
    kpl::String forwardUntil(kpl::StringView string, const kpl::Char sentinal);

    kpl::String forwardUntilLast(const kpl::String& string, const kpl::Char find);
    kpl::String backwardUntilFirst(const kpl::String& string, const kpl::Char find);

    kpl::StringView backwardUntilView(kpl::StringView string, const kpl::Char sentinal);
    kpl::String backwardUntil(kpl::StringView string, const kpl::Char sentinal);

    kpl::String& trimEndMut(kpl::String& string, kpl::Char remove_char);
    kpl::String trimEnd(const kpl::String& string, kpl::Char remove_char);
}


//class ForwardStringIterator : public std::iterator<std::forward_iterator_tag, char>
//{
//  char* m_char;
//public:
//  ForwardStringIterator(char* c) : m_char(c) {}
//  ForwardStringIterator(const ForwardStringIterator& mit) : m_char(mit.m_char) {}

//  ForwardStringIterator& operator++() {++m_char; return *this;}
//  ForwardStringIterator operator++(int) {ForwardStringIterator tmp(*this); operator++(); return tmp;}

//  bool operator==(const ForwardStringIterator& rhs) const {return m_char==rhs.m_char;}
//  bool operator!=(const ForwardStringIterator& rhs) const {return m_char!=rhs.m_char;}

//  char& operator*() {return *m_char;}
//};

//// Prefix for non-fixed bottom
//// ds Dynamic size
//// String_ds
//// 8N = storage amount
//// 8bL = 8 Bits
//// L = Length
//// O = Open
//// F = Fixed

//// String_8bL_o

//#define KPL_DEBUG_STRINGS

//// Mission:
//// Implement a generic, implementation independent string type that can be used in library code

//// How to indicate a type is just an interface handle?
//// String_

//typedef void * String_;


//// How to embed type into variables?

//// Have a numeric value to represent ALL types, or just sub-types
//// Alignment?
//// Something you can turn off for release mode
//// Variable arrays for stack declarations
//// Split size into two, alignment + additional.
//// uint32_t alignment[1]; uint8_t additional[16];(32 bit alignment)
//// Or just uint32_t data[5]

//// Why are interfaces bad?
////

//class Iterable
//{
//    uint8_t * next();
//    void reset_iterator();
//};

//class Stringable
//{
//    const char * to_string();
//};

//class String :
//        public Iterable,
//        public Stringable
//{
//    virtual void as_c_string();
//    virtual void to_c_string();
//    virtual bool is_c_string_viewable();
//    virtual bool is_c_string_convertable();

//    virtual ~String();
//};

//// You can pass in a hardcoded value that can be used to reference the required functions
//// generic_function(data, TYPE_TOKEN);


//// The problem
//// When you setup a function, you need to know how many parameters and how big they are
//// This is hardcoded into the instructions of a function, which is not dynamic

//// You need to be able to specify how a function should be setup in assembly

//struct Interface
//{
//    uint8_t * data;
//    void (*do_something)(uint8_t * data);
//};

//// I can duplicate functions for different types and implement a non-enforced interface
//// The issue with that is you want to be able to cut this out at higher level functions
//// A function written by a user should not have to be duplicated for each permutation of each contract type

//// You could assign functions to data segments of a type
//// You need to implement "fake" type safely
//// E.g typedef void * String
//// Can be used to add zero cost type safetly as opposed to inserting a void * directly.
//// You need to look at your operations from a data point of view

//struct Interface2
//{
//    uint8_t * data;
//    void (*real)(void *);
//    uint32_t func1_offset;
//    uint32_t func2_offset;
//};

//namespace kpl {

//    namespace interface {

//        namespace internal {
//            const uint64_t STRING_START = 100;
//            const uint64_t STRING_END = 110;
//        }

//        struct String
//        {
//            uint64_t type;
//            void * data;
//        };

//        typedef void * FixedArray;
//    }


//    // Q: Property based programming


//    // I can't use zig unless I can find or make a decent graphics library


//    // Array interface
//    uint64_t size();
//    uint64_t space_allocated();
//    uint64_t alignment();
//    uint64_t element_size();
//    uint64_t element_space_required(); // Padding included
//    bool pop();
//    bool push(uint8_t * data, uint64_t size);
//    uint8_t * at(uint32_t index);
//    uint8_t * next();

//    // Save a function pointer with some values hard coded
//    // E.G
//    /*
//     *  void access(void data, uint32_t offset);
//     *  Here we can hardcode offset for the specific data type
//     */

//    // union { uint64_t data, uint64_t (*get_data)(OtherData*); }

//    // Can use a bunch of unions + a bit flag that indicates what each variables is storing
//    // Can also have a variable that is an offset to the data pointer passed

//    // Can use two bits to add more operations

//    // Example operations
//    // function call
//    // direct data
//    // indirect data

//    struct InterfaceCollection
//    {
//        uint8_t union_flag;

//        uint64_t (*size)(void);

//    };

//    struct DataSegment
//    {
//        uint64_t length;
//        uint8_t * data;
//    };

//    struct TextChunk
//    {
//        uint64_t length;
//        char * start;
//    };

//    bool is_last_data_segment(kpl::DataSegment * data)
//    {
//        bool result = (data->length & 0x8000000000000000);

//        if(result) {
//            data->length &= 0x8fffffffffffffff;
//        }

//        return result;
//    }

//    struct DynamicDataRequest
//    {
//        void * request;

//    };

//    struct FixedArray_8
//    {
//        uint64_t length;
//        uint8_t * start;
//    };

//    uint64_t length(kpl::interface::FixedArray array)
//    {
//        return *reinterpret_cast<uint64_t *>(array);
//    }

//    struct Call
//    {
//        void * base_pointer;
//        int32_t offset;
//        uint32_t data_offset;
//    };

//    void number_data_segments(kpl::interface::String data)
//    {
//        (void)data;

//        void (*example_function)(void) = reinterpret_cast<void(*)(void)>(0x00ff00ff00ff00ff);
//        example_function();
//    }

//    void is_equal(kpl::interface::String first, kpl::interface::String second)
//    {
//        assert(first.type >= kpl::interface::internal::STRING_START &&
//               first.type <= kpl::interface::internal::STRING_END &&
//               "'first' parameter of is_equal not a 'string' type");

//        assert(second.type >= kpl::interface::internal::STRING_START &&
//               second.type <= kpl::interface::internal::STRING_END &&
//               "'second' parameter of is_equal not a 'string' type");


//    }

//    // Q: How to remove type from parameter function that has to work on different data types, where they are essentially the same.
//    // A1: Write void * functions
//    // A2: Write void * alias' and use it as a handle
//    // A3: Use a union for the parameter for all the valid types (You don't need a bit flag is they are the same, just choose a random)

//    // Q: What is the tradeoff between additional algorithm complexity + data access (Size of stuff)

//    // You can also precompute values before calling the function, length for example
//    // Maybe you can do something with unions?

//    // T: You can use unions in interfaces if data layout is the same

//    // T: You need a bit for each different memory layout

//    // T: It would be nice if you could reserve a space before a function to setting config variables
//    // Since functions are stored seperately this would not be possable.


//    // Zig advantages
//    // Templates via generics that don't take forever to compile (I assume)

//    struct DoSomethingParams
//    {
//        void * data;
//        uint64_t length; // Pre computed
//        const char * (*as_c_string)(void * data);
//        // Technically this technique can reduce the amount of function data you store for your application
//    };

//    void helper(DoSomethingParams data)
//    {
//        printf("%s\n", data.as_c_string(data.data));
//    }

//    // In this case, you'd need to pass down a type hierarchy...
//    // Unless you have an initialised struct to the interface that you can just pass

//    // I like the idea of keeping functions small + abstract and putting the weight on the data used.

//    void do_something()
//    {
//        helper( { nullptr, 5, nullptr } );
//    }

//    // You could write assembly operation functions..
//    bool is_equal(uint8_t * first, uint8_t * second, uint32_t length);

//    inline uint8_t read_u8(void * data)
//    {
//        return *reinterpret_cast<uint8_t *>(data);
//    }

//    inline uint16_t read_u16(void * data);
//    inline uint32_t read_u32(void * data);
//    inline uint64_t read_u64(void * data);

//    inline uint64_t get_length(FixedArray_8 * array)
//    {
//        return read_u64(reinterpret_cast<void *>(array));
//    }

//    inline void write_u8(void * dst, uint8_t src)
//    {
//        *reinterpret_cast<uint8_t *>(dst) = src;
//    }

//    inline void write_u16(void * dst, uint8_t src)
//    {
//        *reinterpret_cast<uint8_t *>(dst) = src;
//    }

//    inline void write_u32(void * dst, uint8_t src)
//    {
//        *reinterpret_cast<uint8_t *>(dst) = src;
//    }

//    inline void write_u64(void * dst, uint8_t src)
//    {
//        *reinterpret_cast<uint8_t *>(dst) = src;
//    }

//    // Three main things
//    // Read
//    // Compare
//    // Assign

//    inline uint64_t get_length(void * data, uint8_t data_size_bytes)
//    {
//        switch(data_size_bytes)
//        {
//        case 1:
//            return *reinterpret_cast<uint8_t*>(data);
//        case 2:
//            return *reinterpret_cast<uint16_t*>(data);
//        case 3:
//            return *reinterpret_cast<uint32_t*>(data);
//        case 4:
//            return *reinterpret_cast<uint64_t*>(data);
//        default:
//            assert(false && "Invalid data_size_bytes");
//            return 0;
//        }
//    }

//    // You can use function alias' as well, assuming the computer will inline 'useless' functions/
//    // E.g
//    void do_specific_thing()
//    {
//        // do_generic_thing();
//    }



//    namespace literals
//    {
//        const int8_t FIRST_BIGGER = -1;
//        const int8_t SECOND_BIGGER = 1;
//        const int8_t EQUAL = 0;
//    };

//    struct StringRange
//    {
//        const char * text_start;
//        const char * text_end;
//    };

//    struct StringView_64
//    {
//        uint64_t length;
//        const char * start;
//    };

//    struct PackedString_N
//    {
//        uint16_t allocated;
//        uint16_t length;
//        char start[4];
//    };

//    struct PackedString_256
//    {
//        char start[256];
//        uint8_t length;
//    };

//    struct PackedString_1024
//    {
//        uint16_t length;
//        char start[1024];
//    };

//    PackedString_N * initialise_packedstring_n(uint16_t bytes_allocated, uint8_t * placement)
//    {
//        assert(bytes_allocated > 2);
//        assert(placement);

//        *reinterpret_cast<uint16_t*>( placement ) = bytes_allocated - (sizeof(bytes_allocated) * 2);
//        *reinterpret_cast<uint16_t*>( placement + 2) = 0;
//        *reinterpret_cast<char * >( placement + 3) = '\0';

//        return reinterpret_cast<PackedString_N *>(placement);
//    }

//    struct PreAllocString_256
//    {
//        const char * overflow;
//        uint32_t length;
//        char start[256];
//    };

//    char * as_c_string(PreAllocString_256 * string)
//    {
//        assert(string->length < 256);

//        string->start[string->length] = '\0';
//        return string->start;
//    }

//    char * make_c_string(PreAllocString_256 *string, char * placement)
//    {
//        if(placement == nullptr) {
//            placement = new char[string->length + 1];

//            if(! placement)
//            {
//                std::cerr << "Out of memory. Terminating" << std::endl;
//                exit(EXIT_FAILURE);
//            }
//        }

//        memcpy(placement, string->start, 255);
//        memcpy(placement + 255, string->overflow, string->length - 255);

//        placement[string->length] = '\0';

//        return placement;
//    }

////    template <std::size_t STACK_PRE_ALLOC, std::size_t DYNA_BUFFER_INC>
////    struct HybridString
////    {
////        const char * overflow;
////        uint32_t length;
////        char start[STACK_PRE_ALLOC];
////    };

//    uint32_t text_length(StringRange stringView)
//    {
//        return static_cast<uint32_t>(
//                    reinterpret_cast<intptr_t>(stringView.text_end) - reinterpret_cast<intptr_t>(stringView.text_start)
//        );
//    }

//    uint8_t text_length(PackedString_256 string)
//    {
//        uint8_t result = 0;

//        while(result != UINT8_MAX && string.start[result] != '\0') {
//            result++;
//        }

//        return result;
//    }

//    uint32_t text_length(const char * string)
//    {
//        return strlen(string);
//    }

//    uint32_t space_needed(const char * string)
//    {
//        return strlen(string) + 1;
//    }

//    bool is_c_string_convertable(PackedString_256 string)
//    {
//        if(string.length != 255) {
//            return true;
//        }

//        return false;
//    }

//    char * as_c_string(PackedString_256 * string)
//    {
//        string->start[string->length] = '\0';
//        return string->start;
//    }

//    char * into_c_string(PackedString_256 * string)
//    {
//        assert(string->length != 255);

//        string->start[string->length] = '\0';

//        return string->start;
//    }

//    char * c_string(PackedString_256 string, char * placement = nullptr)
//    {
//        if(placement == nullptr)
//        {

//        }
//    }

//    /*  Allocates the space for a new string if placement == nullptr
//     *  Up to the caller to dealloce memory */
//    char * make_duplicate(const char * to_copy, char * placement)
//    {
//        uint32_t required_space = space_needed(to_copy);

//        if(placement == nullptr)
//        {
//            placement = new char[required_space];

//            if(placement == nullptr) {
//                std::cerr << "Out of memory! Terminating" << std::endl;
//                exit(EXIT_FAILURE);
//            }
//        }

//        memcpy(placement, to_copy, required_space);
//        assert( placement[required_space - 1] == '\0' );

//        return placement;
//    }

//    // Will return true even if strings have different lengths
//    int8_t is_equal_lazy(StringView_64 first, StringView_64 second)
//    {
//        uint64_t length = first.length;

//        while(length != 0)
//        {
//            if(first.start[length] != second.start[length])
//            {
//                return (first.start[length] > second.start[length])
//                        ? kpl::literals::FIRST_BIGGER : kpl::literals::SECOND_BIGGER;
//            }

//            length--;
//        }

//        return kpl::literals::EQUAL;
//    }

//    void print(StringView_64 string)
//    {
//        uint64_t len = string.length;
//        // TODO: Turn buffering on

//        while(len != 0)
//        {
//            putc(string.start[len], stdout);
//        }
//    }

//    void print(PackedString_N string)
//    {
//        uint32_t len = string.length;
//        // TODO: Turn buffering on

//        while(len != 0)
//        {
//            putc(string.start[len], stdout);
//        }
//    }

//    void print(PackedString_N ** strings, uint8_t num_strings)
//    {
//        for(uint8_t i = 0; i < num_strings; i++)
//            print(*strings[i]);
//    }
//}

#endif // STRING_H
