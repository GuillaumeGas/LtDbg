#include <sstream>

#include "SymbolsHelper.hpp"
#include "ManglingHelper.hpp"

#include "../Exceptions.hpp"

using namespace ELFIO;

SymbolsHelper * SymbolsHelper::instance = nullptr;

SymbolsHelper::SymbolsHelper() {}

SymbolsHelper * SymbolsHelper::Instance()
{
	if (instance == nullptr)
	{
		instance = new SymbolsHelper();
	}

	return instance;
}

void SymbolsHelper::LoadElf(const std::string & fileName)
{
	if (!_reader.load(fileName))
	{
		throw DbgException("Can't open LtKernel image !");
	}
}

std::string SymbolsHelper::Get(unsigned int addresses[], size_t size)
{
	std::stringstream ss;

	for (unsigned int i = 0; i < size; i++)
	{
        SymbolInfo sym = GetSymbolFromAddr(addresses[i]);
        unsigned int offset = addresses[i] - sym.addr;
		ss << std::hex << addresses[i] << " " << ManglingHelper::Demangle(sym.name) << "+0x" << std::hex << offset << std::endl;
	}

	return ss.str();
}

SymbolInfo SymbolsHelper::GetSymbolFromAddr(unsigned int addr)
{
	Elf_Half sec_num = _reader.sections.size();
	unsigned int max = 0;
	SymbolInfo res;

	for (int i = 0; i < sec_num; i++)
	{
		section * sec = _reader.sections[i];

		if (sec->get_type() == SHT_SYMTAB)
		{
			const symbol_section_accessor symbols(_reader, sec);

			for (int j = 0; j < symbols.get_symbols_num(); j++)
			{
				std::string   name;
				Elf64_Addr    value;
				Elf_Xword     size;
				unsigned char bind;
				unsigned char type;
				Elf_Half      section_index;
				unsigned char other;
				symbols.get_symbol(j, name, value, size, bind,
					type, section_index, other);

				if (!name.empty() && value > max && value <= addr)
				{
					max = (unsigned int)value;
                    res.name = name;
                    res.addr = (unsigned int)value;
				}
			}
		}
	}

    if (res.name.empty())
    {
        res.name = "<symbol not found>";
        res.addr = 0;
    }
	return res;
}

bool SymbolsHelper::GetAddrFromSymbol(std::string symName, unsigned int & addr)
{
	Elf_Half sec_num = _reader.sections.size();

	for (int i = 0; i < sec_num; i++)
	{
		section * sec = _reader.sections[i];

		if (sec->get_type() == SHT_SYMTAB)
		{
			const symbol_section_accessor symbols(_reader, sec);

			for (int j = 0; j < symbols.get_symbols_num(); j++)
			{
				std::string   name;
				Elf64_Addr    value;
				Elf_Xword     size;
				unsigned char bind;
				unsigned char type;
				Elf_Half      section_index;
				unsigned char other;
				symbols.get_symbol(j, name, value, size, bind,
					type, section_index, other);

				if (name == symName)
				{
					addr = (unsigned int)value;
					return true;
				}
			}
		}
	}

	return false;
}