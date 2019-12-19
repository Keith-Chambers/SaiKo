#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <kpl/core.h>

namespace kpl
{

    template <typename T>
    u32 countElementsFromEnd(ContinguousView<T> data, const T& find);

    template <typename T>
    u32 countElementsFromBegin(ContinguousView<T> data, const T& find);

    template <typename T>
    ContinguousView<T> tail(ContinguousView<T> data, u32 length);

    template <typename T>
    ContinguousView<T> head(ContinguousView<T> data, u32 length);

    template <typename T>
    u32 countElementsFromEnd(ContinguousView<T> data, const T& find)
    {
        u32 result = 0;
        const T* sentinal = data.begin() - 1;

        for(const T* item = data.end(); item != sentinal; item--)
        {
            if(item == find) {
                break;
            }

            result++;
        }

        return result;
    }

    template <typename T>
    u32 countElementsFromBegin(ContinguousView<T> data, const T& find)
    {
        u32 result = 0;
        const T* sentinal = data.end() + 1;

        for(const T* item = data.begin(); item != sentinal; item++)
        {
            if(item == find) {
                break;
            }

            result++;
        }

        return result;
    }

    template <typename T>
    ContinguousView<T> tail(ContinguousView<T> data, u32 length)
    {
        return ContinguousView<T>(data.end() - length, data.end());
    }

    template <typename T>
    ContinguousView<T> head(ContinguousView<T> data, u32 length)
    {
        return ContinguousView<T>(data.begin(), length);
    }

    // Function Alias'
    template<typename T>
    constexpr auto elementsFromEnd = tail<T>;

    template<typename T>
    constexpr auto elementsFromBegin = head<T>;
}

#endif // ALGORITHM_H
