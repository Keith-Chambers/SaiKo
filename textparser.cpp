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
