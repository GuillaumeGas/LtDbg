#include <iostream>
#include <sstream>

#include "Dbg.hpp"
#include "Com.hpp"
#include "Exceptions.hpp"
#include "CmdList.hpp"
#include "Utils/Registers.hpp"

#include "..\elfio\elfio\elfio.hpp"
#include "..\elfio\elfio\elf_types.hpp"

// Includes all LtKernel header files
#include "LtKinc/ltkinc.h"

#define EMPTY_RESULT ""

using namespace std;

Command::Command(Dbg * dbg, Com * com, const char * kernelImagePath) : _dbg(dbg), _com(com)
{
	_stackTrace.LoadElf(kernelImagePath);
}

string Command::CmdConnect()
{
	if (_dbg->GetConnectedState())
	{
		return "LtDbg is already connecter to LtKernel !";
	}

    unsigned char res = 0;

	while (1)
	{
		_com->SendByte(1);
		unsigned char res = _com->ReadByte();
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

string Command::CmdRegisters()
{
	KeDebugContext context = { 0 };

	_com->SendByte('r');
	_com->ReadBytes((unsigned char*)&context, sizeof(KeDebugContext));

	RegistersX86 regs(context);

    return regs.ToString();
}

string Command::CmdDisass()
{
	unsigned char buffer[20];
	unsigned int startingAddr = 0;

	_com->SendByte('d');
	_com->ReadBytes((unsigned char *)&startingAddr, 4);
	_com->ReadBytes(buffer, 20);

	_disass.SetInputBuffer(buffer, 20);
	return _disass.Disassemble(startingAddr);
}

string Command::CmdStackTrace()
{
	KeDebugContext context = { 0 };
	u32 * buffer = new u32[2 * sizeof(u32)];

	_com->SendByte('s');
	_com->ReadBytes((unsigned char*)buffer, 2 * sizeof(u32));

	return _stackTrace.Get(buffer, 2);
}