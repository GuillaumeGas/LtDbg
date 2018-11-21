#include <iostream>
#include <sstream>

#include "Dbg.hpp"
#include "Com.hpp"
#include "Exceptions.hpp"
#include "CmdList.hpp"

#include "..\udis\libudis86\extern.h"

#include "..\elfio\elfio\elfio.hpp"
#include "..\elfio\elfio\elf_types.hpp"

// Includes all LtKernel header files
#include "LtKinc/ltkinc.h"

#define EMPTY_RESULT ""

using namespace std;

Command::Command(Dbg * dbg, Com * com) : _dbg(dbg), _com(com) {}

string Command::CmdConnect()
{
	if (_dbg->GetConnectedState())
	{
		return "LtDbg is already connecter to LtKernel !";
	}

	while (1)
	{
		_com->SendByte(1);
		byte res = _com->ReadByte();
		if (res == 1)
			break;
	}

	_dbg->SetConnectedState(true);

	return "Connected to LtKernel !";
}

string Command::CmdStep()
{
	_com->SendByte('s');
	return EMPTY_RESULT;
}

string Command::CmdContinue()
{
	_com->SendByte('c');
	return EMPTY_RESULT;
}

string Command::CmdQuit()
{
	CmdContinue();
	_dbg->SetConnectedState(false);
	return EMPTY_RESULT;
}

/*
kprint("eax = %x, ebx = %x, ecx = %x, edx = %x\n", context->eax, context->ebx, context->ecx, context->edx);
kprint("esp = %x, ebp = %x, esi = %x, edi = %x\n", context->esp, context->ebp, context->esi, context->edi);
kprint("eip = %x", context->eip);
kprint("gs = %x, fs = %x, es = %x, ds = %x\n", context->gs, context->fs, context->es, context->ds, cs);
kprint("eflags = %x (%b)\n", context->eflags, context->eflags);
kprint("cr0 = %x (%b)\n", context->cr0, context->cr0);
kprint("cr2 = %x (%b)\n", context->cr2, context->cr2);
kprint("cr3 = %x (%b)\n", context->cr3, context->cr3);
*/

string Command::CmdRegisters()
{
	KDebugContext context = { 0 };
	stringstream ss;

	_com->SendByte('r');
	_com->ReadBytes((byte*)&context, sizeof(KDebugContext));

	Registers regs(context);

	ss << "eax = " << std::hex << context.eax << ", ";
	ss << "ebx = " << std::hex << context.ebx << ", ";
	ss << "ecx = " << std::hex << context.ecx << ", ";
	ss << "edx = " << std::hex << context.edx << endl;

	ss << "esp = " << std::hex << context.esp << ", ";
	ss << "ebp = " << std::hex << context.ebp << ", ";
	ss << "esi = " << std::hex << context.esi << ", ";
	ss << "edi = " << std::hex << context.edi << endl;

	ss << "eip = " << std::hex << context.eip << endl;

	ss << "gs = " << std::hex << context.gs << ", ";
	ss << "fs = " << std::hex << context.fs << ", ";
	ss << "es = " << std::hex << context.es << ", ";
	ss << "ds = " << std::hex << context.ds << endl;

	return ss.str();
}

string Command::CmdDisass()
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
	return EMPTY_RESULT;
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

string Command::CmdStackTrace()
{
	DebugContext context = { 0 };
	u32 * buffer = new u32[2 * sizeof(u32)];

	_com->SendByte('s');
	_com->ReadBytes((byte*)buffer, 2 * sizeof(u32));

	cout << "0x" << std::hex << buffer[0] << " " << LookForSymbol(buffer[0]) << endl;
	cout << "0x" << std::hex << buffer[1] << " " << LookForSymbol(buffer[1]) << endl;
	return EMPTY_RESULT;
}