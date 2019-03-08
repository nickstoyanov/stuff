#pragma once

#include <vector>

#include "rapidjson/document.h"


namespace utility
{

inline std::vector<std::string> rapidjsonArrayToVector(const rapidjson::Value& rapidjsonArray)
{
    std::vector<std::string> vec;
    if (rapidjsonArray.IsArray())
    {
        for(const auto& value : rapidjsonArray.GetArray())
        {
            vec.push_back(value.GetString());
        }
    }
    return vec;
}

template<typename T>
inline void append(std::vector<T>& first, std::vector<T>& second)
{
    first.insert(first.end(), second.begin(), second.end());
}

}