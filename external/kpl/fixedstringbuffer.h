#ifndef FIXEDSTRINGBUFFER_H
#define FIXEDSTRINGBUFFER_H

#include <cstring>

namespace kpl {

    template <std::size_t N>
    class FixedStringBuffer
    {
    public:
        char data[N + 1];
        std::size_t dataSize = 0;

        bool setSize(std::size_t size)
        {
            if(size >= N)
                return false;

            data[size] = '\0';
            dataSize = size;

            return true;
        }

        void clear()
        {
            std::memset(data, 0, N);
            dataSize = 0;
        }

        constexpr std::size_t bufSize()
        {
            return N;
        }
    };
}



#endif // FIXEDSTRINGBUFFER_H
