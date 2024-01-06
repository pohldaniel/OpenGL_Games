#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <vector>

namespace StringHelper
{
    static std::vector<std::string> splitString(std::string string, std::string delimiter)
    {
        std::vector<std::string> vec;
        size_t last = 0;
        size_t next = 0;

        while ((next = string.find(delimiter, last)) != std::string::npos)
        {
            vec.push_back(string.substr(last, next - last));
            last = next + delimiter.length();
        }
        vec.push_back(string.substr(last));
        return vec;
    }
}