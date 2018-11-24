#include <sstream>

#include "StackTrace.hpp"

#include "../Exceptions.hpp"

using namespace ELFIO;

void StackTrace::LoadElf(const std::string & fileName)
{
	if (!_reader.load(fileName))
	{
		throw DbgException("Can't open LtKernel image !");
	}
}

std::string StackTrace::Get(unsigned int addresses[], size_t size)
{
	std::stringstream ss;

	for (unsigned int i = 0; i < size; i++)
	{
		ss << std::hex << addresses[i] << " " << LookForSymbol(addresses[i]) << std::endl;
	}

	return ss.str();
}

std::string StackTrace::LookForSymbol(unsigned int addr)
{
	Elf_Half sec_num = _reader.sections.size();
	unsigned int max = 0;
	std::string res = "";
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

				if (!name.empty() && value > max && value < addr)
				{
					max = (unsigned int)value;
					res = name;
				}
			}
		}
	}

	if (res.empty())
		res = "<symbol not found>";
	return res;
}