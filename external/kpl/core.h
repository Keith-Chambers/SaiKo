#ifndef CORE_H
#define CORE_H

#include <stdint.h>

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

//#define KPL_USE_QT 1
//#define KPL_USE_STL 0
//#define KPL_USE_KPL 0

#if(KPL_USE_QT)

    #include <QDebug>
    #include <QString>
    #include <QDir>
    #include <QDirIterator>
    #include <QChar>

#else
    #include <string>
    #include <filesystem>
    #include <vector>
#endif

namespace kpl
{
    #if(KPL_USE_QT)
        using String = QString;
        using Directory = QDir;
        using Char = QChar;

        template<typename T>
        using Iterator = QString::iterator;

        template<typename T>
        using List = QList<T>;
    #else
        using String = std::string;
        using Directory = std::filesystem::directory_entry;
        using Char = char;

        template<typename T>
        using List = std::vector<T>;
    #endif

    class ContinguousMemory
    {
    public:
        ContinguousMemory(u8 *memory, u32 size_bytes)
            : m_memory_start {memory}, m_size_bytes {size_bytes}, m_flags {0}
        {}

        static ContinguousMemory nullMemory()
        {
            return ContinguousMemory { nullptr, 0};
        }

        bool isNull() const {
            return m_memory_start == nullptr;
        }
//    private:
        u8* m_memory_start;
        u32 m_size_bytes;
        u32 m_flags;
    };

    static_assert(sizeof(ContinguousMemory) == 16);

    template <typename T>
    class ContinguousView
    {
    public:
        ContinguousView(const T* start, const T* end)
            : m_start{start}, m_len{end - start}{}

        ContinguousView(const T* start, uint64_t len)
            : m_start{start}, m_len{len}{}

        const T* begin() const
        {
            return m_start;
        }

        const T* end() const
        {
            return m_start + m_len;
        }

    private:
        const T* m_start;
        uint64_t m_len;
    };

    enum class IterationDirection { FORWARD, BACKWARD };

    template <typename T, IterationDirection D>
    class IterableRange
    {
    public:

        class iterator {
           public:
             iterator(T * ptr): ptr(ptr){}
             iterator operator++()
             {
                 if(D == IterationDirection::FORWARD) {
                     ptr++;
                     return *this;
                 } else {
                     ptr--;
                     return *this;
                 }
             }

             bool operator!=(const iterator & other) const { return ptr != other.ptr; }
             const T& operator*() const { return *ptr; }
           private:
             T* ptr;
        };

        IterableRange(T* begin, T* end)
            : m_data{begin, end}{}

        iterator begin() const
        {
            return iterator(m_data.begin());
        }

        iterator end() const
        {
            return iterator(m_data.end());
        }

    private:
        ContinguousView<T> m_data;
    };


    using StringView = ContinguousView<kpl::Char>;
}

#endif // CORE_H
