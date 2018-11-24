#pragma once

#include <string>

#include "..\elfio\elfio\elfio.hpp"
#include "..\elfio\elfio\elf_types.hpp"

class StackTrace
{
public:
	void LoadElf(const std::string & fileName);
	std::string Get(unsigned int addresses[], size_t size);

private:
	std::string LookForSymbol(unsigned int addr);

	ELFIO::elfio _reader;
};