#include "kantu/string.hpp"

std::string kantu::to_lower(const std::string& str)
{
    std::string lower_str = str;
    for (size_t i = 0; i < str.size(); i++)
    {
        if (isalpha(str[i]))
        {
            lower_str[i] = tolower(str[i]);
        }
    }
    return lower_str;
}

std::string& kantu::replace_all(std::string& src, const std::string& old_value, const std::string& new_value)
{
    for (std::string::size_type pos(0); pos != std::string::npos; pos += new_value.length())
    {
        if ((pos = src.find(old_value, pos)) != std::string::npos)
        {
            src.replace(pos, old_value.length(), new_value);
        }
        else
        {
            break;
        }
    }
    return src;
}