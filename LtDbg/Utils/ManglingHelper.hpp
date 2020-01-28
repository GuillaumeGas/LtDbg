#pragma once

#include <string>
#include <sstream>

class ManglingHelper
{
public:
    static std::string Demangle(const std::string&);

private:
    static bool _demangleUsingAbi(const std::string &, std::string &outString);
    static void _visit_Z(const std::string&, std::stringstream&);
    static void _visitName(const std::string&, std::stringstream&);
    static void _visitNamespace(const std::string&, std::stringstream&);
    static int _visitNumber(const std::string&, int& nbDigits);
};
