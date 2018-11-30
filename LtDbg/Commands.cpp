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
		return "Too many arguments for disassemble command";
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

string Command::CmdMemory(vector<string> * args)
{
	const unsigned int DEFAULT_NB_BYTES = 50;
	const unsigned int NB_BYTES_PER_LINE = 10;

	unsigned int addr = 0;
	unsigned int size = DEFAULT_NB_BYTES;
	size_t nbArgs = args->size();
	unsigned char * buffer = nullptr;
	stringstream ss;
	char memoryAccessible = 0;
	
	if (nbArgs < 1)
	{
		return "Missing argument 'address'";
	}

	string stringAddr = (*args)[0];
	if (stringAddr.size() > 2 && stringAddr[0] == '0' && stringAddr[1] == 'x')
	{
		addr = std::stoul(stringAddr.substr(2, stringAddr.size()), nullptr, 16);
	}
	else
	{
		addr = std::stoul(stringAddr, nullptr, 16);
	}

	if (nbArgs > 2)
	{
		throw DbgException("Too many arguments for memory command");
	}
	else if (nbArgs == 2)
	{
		size = std::stoi((*args)[1]);
	}

	buffer = new unsigned char[size];
	if (buffer == nullptr)
	{
		throw DbgException("CmdMemory() : Can't allocate memory for buffer");
	}

	_com->SendByte(CMD_MEMORY);
	_com->SendBytes((unsigned char *)&addr, sizeof(unsigned int));
	_com->SendBytes((unsigned char *)&size, sizeof(unsigned int));

	memoryAccessible = _com->ReadByte();
	if (memoryAccessible == 0)
	{
		return "<memory not available>";
	}

	_com->ReadBytes(buffer, size);

	ss << std::hex << addr << " ";
	for (unsigned int i = 0; i < size; i++)
	{
		if (i % NB_BYTES_PER_LINE == 0 && i != 0)
		{
			addr += NB_BYTES_PER_LINE;

			ss << endl;
			ss << std::hex << addr << " ";
		}

		if (buffer[i] <= 0xf)
			ss << std::hex << "0" << (unsigned int)buffer[i] << " ";
		else
			ss << std::hex << (unsigned int)buffer[i] << " ";
	}

	return ss.str();
}