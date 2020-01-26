#pragma once

#include <string>
#include <map>

#include "LtDbg\elfio\elfio\elfio.hpp"
#include "LtDbg\elfio\elfio\elf_types.hpp"

struct SymbolInfo
{
    unsigned int addr;
    std::string name;
};

class SymbolsHelper
{
public:
	static SymbolsHelper * Instance();

    void LoadSymbolsFromFiles(const std::vector<std::string> & filePaths);
    std::string Get(const char * binaryName, unsigned int addresses[], size_t size);

    SymbolInfo GetSymbolFromAddr(const char * binaryName, unsigned int addr);
    bool GetAddrFromSymbol(const char * binaryName, std::string symName, unsigned int & addr);


private:
	SymbolsHelper();
	static SymbolsHelper * instance;

    void ClearReader();

    std::map<std::string, ELFIO::elfio*> _reader;
};
