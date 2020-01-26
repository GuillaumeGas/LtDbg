#include "ManglingHelper.hpp"

#include <cstring>

using namespace std;

string ManglingHelper::Demangle(const string & mangledName)
{
    const char * MANGLE_PREFIX = "_Z";
    stringstream ss;

    if (mangledName.find_first_of(MANGLE_PREFIX, 0) != 0)
        return mangledName;

    _visit_Z(mangledName.substr(strlen(MANGLE_PREFIX), mangledName.length()), ss);

    return ss.str();
}

void ManglingHelper::_visit_Z(const string & name, stringstream & result)
{
    const char * NAMESPACE_PREFIX = "N";

    if (name.find_first_of(NAMESPACE_PREFIX, 0) != 0)
        _visitName(name, result);
    else
        _visitNamespace(name.substr(strlen(NAMESPACE_PREFIX), name.length()), result);
}

void ManglingHelper::_visitName(const string & name, stringstream & result)
{
    result << name;
}

void ManglingHelper::_visitNamespace(const string & name, stringstream & result)
{
    int nbDigitNamespace = 0;
    int namespaceSize = _visitNumber(name, nbDigitNamespace);

    if (namespaceSize == 0)
    {
        _visitName(name, result);
    }
    else
    {
        _visitName(name.substr(nbDigitNamespace, namespaceSize), result);

        result << "::";

        int nbDigitFuncName = 0;
        int funcSize = _visitNumber(name.substr(nbDigitNamespace + namespaceSize), nbDigitFuncName);

        _visitName(name.substr(nbDigitNamespace + namespaceSize + nbDigitFuncName, funcSize), result);
    }
}

int ManglingHelper::_visitNumber(const string & name, int & nbDigits)
{
    stringstream ss;

    for (int i = 0; i < name.length(); i++)
    {
        if (name[i] < '0' || name[i] > '9')
            break;

        ss << name[i];
        nbDigits++;
    }

    return stoi(ss.str());
}
