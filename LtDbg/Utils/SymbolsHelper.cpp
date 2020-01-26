#include <sstream>

#include "SymbolsHelper.hpp"
#include "ManglingHelper.hpp"
#include "StringUtils.hpp"

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

void SymbolsHelper::LoadSymbolsFromFiles(const std::vector<std::string> & filePaths)
{
    /*
     * TODO : improve this mechanism to avoid parsing an elf for each symbol research
     * */

    ClearReader();

    for(const std::string & it: filePaths)
    {
        ELFIO::elfio * elf = new ELFIO::elfio;
        if (elf->load(it))
        {
            std::string binName = StringUtils::GetFileNameFromPath(it);
            _reader.insert(std::pair<std::string, ELFIO::elfio*>(binName, elf));
            std::cout << "Loaded " << binName << std::endl;
        }
        else
        {
            std::cout << "Failed to load " << it << std::endl;
        }
    }
}

void SymbolsHelper::ClearReader()
{
    for(auto it = _reader.begin(); it != _reader.end(); it++)
    {
        if (it->second != nullptr)
            delete it->second;
    }
    _reader.clear();
}

std::string SymbolsHelper::Get(const char * binaryName, unsigned int addresses[], size_t size)
{
	std::stringstream ss;

	for (unsigned int i = 0; i < size; i++)
	{
        SymbolInfo sym = GetSymbolFromAddr(binaryName, addresses[i]);
        unsigned int offset = addresses[i] - sym.addr;
		ss << std::hex << addresses[i] << " " << ManglingHelper::Demangle(sym.name) << "+0x" << std::hex << offset << std::endl;
	}

	return ss.str();
}

SymbolInfo SymbolsHelper::GetSymbolFromAddr(const char * binaryName, unsigned int addr)
{
    SymbolInfo res;

    auto it = _reader.find(binaryName);
    if (it != _reader.end())
    {
        ELFIO::elfio* elf = it->second;
        Elf_Half sec_num = elf->sections.size();
        unsigned int max = 0;

        for (int i = 0; i < sec_num; i++)
        {
            section * sec = elf->sections[i];

            if (sec->get_type() == SHT_SYMTAB)
            {
                const symbol_section_accessor symbols(*elf, sec);

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
    }

    if (res.name.empty())
    {
        res.name = "<symbol not found>";
        res.addr = 0;
    }
	return res;
}

bool SymbolsHelper::GetAddrFromSymbol(const char * binaryName, std::string symName, unsigned int & addr)
{
    auto it = _reader.find(binaryName);
    if (it != _reader.end())
    {
        ELFIO::elfio* elf = it->second;
        Elf_Half sec_num = elf->sections.size();

        for (int i = 0; i < sec_num; i++)
        {
            section * sec = elf->sections[i];

            if (sec->get_type() == SHT_SYMTAB)
            {
                const symbol_section_accessor symbols(*elf, sec);

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
    }

	return false;
}
