#include "textparser.h"

ParseValue parseSaikoLine(std::string line, const std::vector<ParseOption> validOptions)
{
    for(const ParseOption& opt : validOptions)
    {
        if(line.compare(0, opt.optionName.size(), opt.optionName) == 0) {
            return ParseValue {
                line.substr(opt.optionName.size() + 1),
                opt.optionType
            };
        }
    }

    return ParseValue {
        "",
        SaikOption::INVALID
    };
}

QString frontImageForFolder(QDir directory)
{
    ParseValue parseSaikoLine(std::string line, const std::vector<ParseOption> validOptions);

    if(!directory.exists(".saik")) {
        qDebug() << "Warning: No .saik file found for directory -> " << directory.path();
        return "";
    }

    QFile file(directory.path() + "/.saik");

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "Failed to open " << file.fileName();
        return "";
    }

    while(!file.atEnd())
    {
        std::string line = file.readLine().toStdString();
        ParseValue option = parseSaikoLine(line, PARSE_OPTIONS);
        if(option.optionType != SaikOption::FRONT_COVER) {
            continue;
        }

        trimEnd(option.value);
        trimFront(option.value);

        return directory.absolutePath() + "/" + QString::fromStdString(option.value);
    }

    return "";
}

QString removeExtension(const QString& fileName)
{
    int removeEnd = 0;

    for(int i = fileName.size() - 1; i != 0; i++)
    {
        if(fileName[i] != '.') {
            removeEnd++;
            continue;
        }

        break;
    }

    if(removeEnd == fileName.size()) {
        return fileName;
    }

    return fileName.right(fileName.size() - removeEnd);
}

std::string removeExtension(const std::string& fileName)
{
    unsigned long removeEnd = 0;

    for(unsigned long i = fileName.size() - 1; i != 0; i++)
    {
        if(fileName[i] != '.') {
            removeEnd++;
            continue;
        }

        break;
    }

    if(removeEnd == fileName.size()) {
        return fileName;
    }

    return fileName.substr(fileName.size() - removeEnd, std::string::npos);
}

void trimEnd(std::string& str)
{
    int remove = 0;

    for(size_t i = str.size() - 1; i != 0; i--)
    {
        if(str[i] == ' ' || str[i] == '\n' || str[i] == '\t') {
            remove++;
            continue;
        }
        break;
    }

    str.erase(str.size() - remove);
}

void trimFront(std::string& str)
{
    size_t remove = 0;

    for(size_t i = 0; i < str.size(); i++)
    {
        if(str[i] == ' ' || str[i] == '\n' || str[i] == '\t') {
            remove++;
            continue;
        }

        break;
    }

    str.erase(0, remove);
}

std::string trim(const std::string& str)
{
    size_t first = str.find_first_not_of(' ');
    if(std::string::npos == first)
        return str;

    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}
