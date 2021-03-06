#pragma once

#include <vector>
#include <string>

class StringUtils
{
public:
	static std::vector<std::string> Split(std::string str, char delimiter);
    static std::string GetFileNameFromPath(const std::string & path);
};
