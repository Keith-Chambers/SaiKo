#ifndef TEXTPARSER_H
#define TEXTPARSER_H

#include <string>
#include <vector>

enum class SaikOption {
    FRONT_COVER,
    BACK_COVER,
    INVALID
};

struct ParseOption
{
    std::string optionName;
    SaikOption optionType;
};

struct ParseValue
{
    std::string value;
    SaikOption optionType;
};

const std::vector<ParseOption> PARSE_OPTIONS {
    {
        "front_cover",
        SaikOption::FRONT_COVER
    },
    {
        "back_cover",
        SaikOption::BACK_COVER
    }
};

void trimFront(std::string& str);
void trimEnd(std::string& str);
ParseValue parseSaikoLine(std::string line, const std::vector<ParseOption> validOptions);

#endif // TEXTPARSER_H
