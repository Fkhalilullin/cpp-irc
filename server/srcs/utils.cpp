#include "../includes/utils.hpp"

const std::string utils::toUpper(const std::string &str)
{
    std::string tmp(str.length(), 0);

    for (size_t i = 0; i < str.length(); ++i)
        tmp[i] = std::toupper(str[i]);
    return (tmp);
}
