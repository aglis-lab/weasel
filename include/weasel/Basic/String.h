#include <string>
#include <vector>

std::vector<std::string> splitString(std::string src, std::string delimiter)
{
    if (delimiter.empty())
    {
        return {src};
    }

    std::vector<std::string> arrStr;
    size_t pos = 0;
    std::string token;
    while ((pos = src.find(delimiter)) != std::string::npos)
    {
        token = src.substr(0, pos);
        arrStr.push_back(token);

        src.erase(0, pos + delimiter.length());
    }
    arrStr.push_back(src);

    return arrStr;
}
