#ifndef UTIL_H
#define UTIL_H

#define ANAGAMI_DEBUG

#ifdef ANAGAMI_DEBUG

#include <fmt/format.h>
#include <assert.h>
#endif

#include <string>
#include <fstream>
#include <cstring>
#include <vector>

namespace kpl {

    class Util
    {
    public:
        static const std::string constEmptyString;

        // void * nextAlignmentFor(void * address, uint16_t alignment); // TODO

        /* Warning: Not safe. Check memory block ranges before invoking */
        static bool dataRightShift(uint8_t * memoryBlockStart, uint16_t numElements, size_t elementSize, uint16_t shiftSize = 1);
        static void indentString(std::string& targetString, const std::string& indentationString = "    ", uint16_t indentLevels = 1);
        static std::string lowerFirstChar(std::string str);
        static std::string upperFirstChar(std::string str);
        static std::string makeStringUpper(std::string str);
        static std::string makeStringLower(std::string str);
        static std::string encloseQuotes(const std::string str);

        template <typename T>
        static bool uniquePushBack(std::vector<T> vec, const T& value)
        {
            for(const T& val : vec)
                if( val == value )
                    return false;

            vec.push_back( value );
            return true;
        }

        static void createFile(const std::string& path, const std::string& contents);
        static void prependFile(const std::string& filePath, std::string content);
        static void copyFileUnix(const std::string& srcPath, const std::string& destPath, const std::string& fileName);
        static bool appendContentToFile(const std::string& filePath, const std::string& contents);
        static std::string loadTextFile(const std::string& filePath);

        Util();
    };
}

#endif // UTIL_H
