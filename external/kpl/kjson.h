#ifndef KJSON_H
#define KJSON_H

#include <iostream>
#include <vector>
#include <functional>

#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"

#include "kpl/kfile.h"

namespace kpl
{
    namespace rapidjson {
        bool parseDocumentFromFile(std::string filePath, ::rapidjson::Document& document);
        bool checkForMemberList(::rapidjson::Document& document, const char ** memberList, size_t listLength);
        bool parseStringArray(::rapidjson::Value & root, std::vector<std::string>& outputArray);
        bool requireMember(::rapidjson::Value& obj, std::string memberName);

        template <typename T>
        bool parseStringArrayAndApply(::rapidjson::Value& root, std::function<void(T)> apply)
        {
            if(! root.IsArray())
            {
                std::cout << "Error: rapidjson::Value param passed to kpl::rapidjson::parseStringArrayAndApply is not array" << std::endl;
                return false;
            }

            for(::rapidjson::SizeType i = 0; i < root.Size(); i++)
                apply( T { root[i].GetString() } );

            return true;
        }
    }
}

#endif // KJSON_H
