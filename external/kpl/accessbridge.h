#ifndef ACCESSBRIDGE_H
#define ACCESSBRIDGE_H

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <stdlib.h>

#include "kpl/datasource.h"

namespace kpl
{
    class AccessBridge
    {
    public:
        static AccessBridge *getInstance();
        bool registerDataSource(std::string pName, DataSource * pDataSource);
        void *fetchFromDataSource(std::string pName);
        
        ~AccessBridge();
    private:
        AccessBridge();
        
        static AccessBridge *mInstance;
        std::unordered_map<std::string, DataSource *> mDataSourceMappings;
    };
}

#endif // ACCESSBRIDGE_H
