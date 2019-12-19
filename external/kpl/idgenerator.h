#ifndef IDGENERATOR_H
#define IDGENERATOR_H

#include <string>
#include <map>

#include <cstdint>

namespace kpl {

    class IDGenerator
    {
    public:
        IDGenerator();

        uint16_t getID(const std::string& key);

    private:
        uint16_t mNextID;
        std::map<std::string, uint16_t> mMap;
    };
}



#endif // IDGENERATOR_H
