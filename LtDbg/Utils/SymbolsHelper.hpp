#pragma once

#include <string>

#include "..\elfio\elfio\elfio.hpp"
#include "..\elfio\elfio\elf_types.hpp"

struct SymbolInfo
{
    unsigned int addr;
    std::string name;
};

class SymbolsHelper
{
public:
	static SymbolsHelper * Instance();

	void LoadElf(const std::string & fileName);
	std::string Get(unsigned int addresses[], size_t size);

	SymbolInfo GetSymbolFromAddr(unsigned int addr);
	bool GetAddrFromSymbol(std::string symName, unsigned int & addr);


private:
	SymbolsHelper();
	static SymbolsHelper * instance;

	ELFIO::elfio _reader;
};