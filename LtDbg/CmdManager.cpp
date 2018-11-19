#include <iostream>

#include "CmdManager.hpp"
#include "Exceptions.hpp"
#include "Dbg.hpp"

#include "..\udis\libudis86\extern.h"

#include "..\elfio\elfio\elfio.hpp"
#include "..\elfio\elfio\elf_types.hpp"

// Includes all LtKernel header files
#include "LtKinc/ltkinc.h"

using namespace std;

CommandManager::CommandManager(Dbg * dbg, Com * com) : _dbg(dbg), _com(com) 
{
	_commands[CMD_CONNECT] = std::bind(&CommandManager::CmdConnect, this);
	_commands[CMD_STEP] = std::bind(&CommandManager::CmdStep, this);
	_commands[CMD_CONTINUE] = std::bind(&CommandManager::CmdContinue, this);
	_commands[CMD_QUIT] = std::bind(&CommandManager::CmdQuit, this);
	_commands[CMD_REGISTERS] = std::bind(&CommandManager::CmdRegisters, this);
	_commands[CMD_DISASS] = std::bind(&CommandManager::CmdDisass, this);
	_commands[CMD_STACK_TRACE] = std::bind(&CommandManager::CmdStackTrace, this);
}

std::function<void()> & CommandManager::operator[](Command cmd)
{
	return _commands[cmd];
}

std::function<void()> & CommandManager::operator[](const std::string & str)
{
	Command cmd = GetCmdFromStr(str);

	if (cmd == CMD_UNKNOWN)
		throw DbgException("Unknwo command !");

	if (!CommandExists(cmd))
		throw DbgException("Not implemented command !");

	return _commands[cmd];
}

bool CommandManager::CommandExists(const std::string & command) const
{
	Command cmd = GetCmdFromStr(command);

	if (cmd == CMD_UNKNOWN)
		return false;

	return _commands.find(cmd) != _commands.end();
}

bool CommandManager::CommandExists(Command command) const
{
	return _commands.find(command) != _commands.end();
}

void CommandManager::CmdConnect()
{
	if (_dbg->GetConnectedState())
	{
		cout << "LtDbg is already connecter to LtKernel !" << endl;
		return;
	}

	while (1)
	{
		_com->SendByte(1);
		byte res = _com->ReadByte();
		if (res == 1)
			break;
	}
	
	cout << "Connected to LtKernel !" << endl;
	_dbg->SetConnectedState(true);
}

void CommandManager::CmdStep()
{
	_com->SendByte('s');
}

void CommandManager::CmdContinue()
{
	_com->SendByte('c');
}

void CommandManager::CmdQuit()
{
	CmdContinue();
	_dbg->SetConnectedState(false);
}

void CommandManager::CmdRegisters()
{
	DebugContext context = { 0 };

	_com->SendByte('r');
	_com->ReadBytes((byte*)&context, sizeof(DebugContext));

	cout << "eip : " << std::hex << context.eip << endl;
}

void CommandManager::CmdDisass()
{
	byte buffer[20];
	ud_t ud_obj;

	_com->SendByte('d');
	_com->ReadBytes(buffer, 20);

	ud_init(&ud_obj);
	ud_set_input_buffer(&ud_obj, buffer, 20);
	ud_set_mode(&ud_obj, 64);
	ud_set_syntax(&ud_obj, UD_SYN_INTEL);

	while (ud_disassemble(&ud_obj)) 
	{
		ud_mnemonic_code code = ud_insn_mnemonic(&ud_obj);
		if (code != UD_Iinvalid)
			printf("\t%s\n", ud_insn_asm(&ud_obj));
	}
}

using namespace ELFIO;

static std::string LookForSymbol(u32 addr)
{
	elfio reader;

	if (!reader.load("C:\\Users\\Guillaume\\Documents\\Visual Studio 2017\\Projects\\ltkernel\\iso\\boot\\ltkernel.img"))
	{
		throw DbgException("Can't open LtKernel image !");
	}

	/*std::cout << "Openning LtKernel ELF image :" << std::endl;

	std::cout << "  Class : ";
	if (reader.get_class() == ELFCLASS32)
		std::cout << "ELF32" << std::endl;
	else
		std::cout << "ELF64" << std::endl;

	std::cout << "  ELF file encoding : ";
	if (reader.get_encoding() == ELFDATA2LSB)
		std::cout << "Little endian" << std::endl;
	else
		std::cout << "Big endian" << std::endl;

	std::cout << std::endl;*/

	Elf_Half sec_num = reader.sections.size();
	u32 max = 0;
	std::string res = "";
	for (int i = 0; i < sec_num; i++)
	{
		section * sec = reader.sections[i];

		if (sec->get_type() == SHT_SYMTAB)
		{
			const symbol_section_accessor symbols(reader, sec);

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
					max = (u32)value;
					res = name;
				}
			}
		}
	}

	if (res.empty())
		res = "<symbol not found>";
	return res;
}

void CommandManager::CmdStackTrace()
{
	DebugContext context = { 0 };
	u32 * buffer = new u32[2 * sizeof(u32)];

	_com->SendByte('s');
	_com->ReadBytes((byte*)buffer, 2 * sizeof(u32));

	cout << "0x" << std::hex << buffer[0] << " " << LookForSymbol(buffer[0]) << endl;
	cout << "0x" << std::hex << buffer[1] << " " << LookForSymbol(buffer[1]) << endl;
}