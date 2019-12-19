#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <cstring>
#include <iostream>

namespace kpl
{

    class Logger
    {
    public:
        static Logger * getInstance(size_t reserveBytes = 1000);

        bool log(const char * text, size_t size);
        bool log(char * text, size_t size);

        bool log(const char * text);
        bool log(char * text);

        bool logSameLine(const char * text);
        bool logSameLine(char * text);

        size_t getContentSize();
        size_t getBufferSize();

        void clear();
        char * cString(size_t * stringSize = nullptr);
        bool copyToCBuffer(char * buffer, size_t bufferSize);
    private:
        Logger(size_t reserveBytes);

        bool fixedCStringAppend(const char * newLog, size_t size);
        bool fixedCStringAppendNL(const char * newLog, size_t size);
        void shiftCStringLeft(char * string, size_t *stringSize, size_t shiftAmount);

        static Logger * mInstance;
        char * mLogData;
        // Physical buffer size
        size_t mLogDataBufferSize;
        // Number of data elements
        size_t mLogDataContentSize;
    };

}
#endif // LOGGER_H
