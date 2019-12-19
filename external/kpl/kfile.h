#ifndef KFILE_H
#define KFILE_H

#include <string>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <iostream>

namespace kpl
{
    bool fileExists(const std::string& filePath);
    std::string loadTextFile(std::string filePath);
    bool createFile(const std::filesystem::path, bool force);
    bool truncateFile(const std::filesystem::path path);
}
#endif // KFILE_H
