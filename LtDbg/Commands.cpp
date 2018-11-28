#include <iostream>
#include <sstream>

#include "Dbg.hpp"
#include "Com.hpp"
#include "Exceptions.hpp"
#include "Commands.hpp"
#include "Utils/Registers.hpp"
#include "Utils/SymbolsHelper.hpp"

#include "..\elfio\elfio\elfio.hpp"
#include "..\elfio\elfio\elf_types.hpp"

// Includes all LtKernel header files
#include "LtKinc/ltkinc.h"

#define EMPTY_RESULT ""

using namespace std;

Command::Command(Dbg * dbg, Com * com, const char * kernelImagePath) : _dbg(dbg), _com(com)
{
	SymbolsHelper::Instance()->LoadElf(kernelImagePath);
}

string Command::CmdConnect(vector<string> * args)
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

string Command::CmdStep(vector<string> * args)
{
	_com->SendByte(CMD_STEP);

	return _CmdDisass(1);
}

string Command::CmdContinue(vector<string> * args)
{
	_com->SendByte(CMD_CONTINUE);
	return EMPTY_RESULT;
}

string Command::CmdQuit(vector<string> * args)
{
	CmdContinue(args);
	_dbg->SetConnectedState(false);
	return EMPTY_RESULT;
}

string Command::CmdRegisters(vector<string> * args)
{
	KeDebugContext context = { 0 };

	_com->SendByte(CMD_REGISTERS);
	_com->ReadBytes((unsigned char*)&context, sizeof(KeDebugContext));

	RegistersX86 regs(context);

    return regs.ToString();
}

string Command::CmdDisass(vector<string> * args)
{
	const unsigned int DEFAULT_NB_INST = 10;
	unsigned int size = DEFAULT_NB_INST; // x inst to disassemble
	size_t nbArgs = args->size();

	if (nbArgs > 1)
	{
		return "Too many argments for disassemble command";
	}
	else if (nbArgs == 1)
	{
		size = (unsigned int)std::stoi((*args)[0]);
	}

	_com->SendByte(CMD_DISASS);

	return _CmdDisass(size);
}

string Command::_CmdDisass(unsigned int size)
{
	unsigned char * buffer = nullptr;
	unsigned int startingAddr = 0;
	unsigned int bufferSize = size * DEFAULT_ASM_BUFFER_SIZE;

	buffer = new unsigned char[bufferSize];
	if (buffer == nullptr)
	{
		throw DbgException("_CmdDisass() : Failed to allocate memory for buffer");
	}

	_com->SendBytes((unsigned char *)&size, sizeof(unsigned int));

	_com->ReadBytes((unsigned char *)&startingAddr, sizeof(unsigned int));
	_com->ReadBytes(buffer, bufferSize);

	_disass.SetInputBuffer(buffer, bufferSize);
	return _disass.Disassemble(startingAddr, size);
}

string Command::CmdStackTrace(vector<string> * args)
{
	KeDebugContext context = { 0 };
	unsigned char * buffer = nullptr;
	unsigned int bufferSize = 0;

	_com->SendByte(CMD_STACK_TRACE);
	_com->ReadBytes((unsigned char *)&bufferSize, sizeof(unsigned int));

	buffer = new unsigned char[bufferSize];

	_com->ReadBytes((unsigned char*)buffer, bufferSize);

	return SymbolsHelper::Instance()->Get((unsigned int *)buffer, bufferSize / sizeof(unsigned int));
}