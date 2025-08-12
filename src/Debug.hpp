#pragma once

#include <string>
#include <iostream>

inline void DebugLog(const std::string &message)
{
    std::cout << "[DEBUG] " << message << std::endl;
}
