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

#include "LtKinc/ltkinc.h"

#define EMPTY_RESULT ""

using namespace std;

Command::Command(Dbg * dbg, Com * com, const string kernelImagePath) : _dbg(dbg), _com(com)
{
	SetSymbolsPath(kernelImagePath);
}

void Command::SetSymbolsPath(const std::string symbolsFileName)
{
	if (!symbolsFileName.empty())
		SymbolsHelper::Instance()->LoadElf(symbolsFileName.c_str());
}

DbgResponsePtr Command::CmdConnect(vector<string> * args, KeDebugContext * context)
{
	if (_dbg->IsConnected())
	{
		string msg = "LtDbg is already connecter to LtKernel !";
		return DbgResponse::New(CMD_CONNECT, DBG_STATUS_ALREADY_CONNECTED, msg, context);
	}

    unsigned char res = 0;

	while (1)
	{
		_com->SendByte(1);
		unsigned char res = _com->ReadByte();
		if (res == 1)
			break;
	}

	KeDebugRequest req;
	req.command = CMD_CONNECT;
	req.paramSize = 0;
	req.param = nullptr;

	KeDebugResponse response = _com->SendRequest(req);

	if (response.header.status != DBG_STATUS_SUCCESS)
	{
		string msg = "Connect command failed !";
		return DbgResponse::New(CMD_CONNECT, response.header.status, msg, &(response.header.context));
	}

	_dbg->IsConnected(true);

	string msg = "Connected to LtKernel !";
	return DbgResponse::New(CMD_CONNECT, DBG_STATUS_SUCCESS, msg, &(response.header.context));
}

DbgResponsePtr Command::CmdStep(vector<string> * args, KeDebugContext * context)
{
	KeDebugRequest req = { CMD_STEP, 0, nullptr };

	KeDebugResponse res = _com->SendRequest(req);

	if (res.header.status == DBG_STATUS_SUCCESS)
	{
		// 1 instruction à désassembler
		const unsigned int NB_INST = 1;
		return _CmdDisass(NB_INST, &res.header.context);
	}
	else
	{
		return DbgResponse::New(CMD_STEP, res.header.status, "Step command failed !", context);
	}
}

DbgResponsePtr Command::CmdContinue(vector<string> * args, KeDebugContext * context)
{
	KeDebugRequest req = { CMD_CONTINUE, 0, nullptr };
	KeDebugResponse res = _com->SendRequest(req);

	if (res.header.status == DBG_STATUS_SUCCESS)
	{
		// TODO : On doit gérer le type d'event qui a fait break le noyau
		return DbgResponse::New(CMD_CONTINUE, res.header.status, "Kernel broke ! Breakpoint hit ?", &res.header.context);
	}
	else      
	{
		return DbgResponse::New(CMD_CONTINUE, res.header.status, "Continue command failed !", context);
	}
}

DbgResponsePtr Command::CmdQuit(vector<string> * args, KeDebugContext * context)
{
	KeDebugRequest req = { CMD_QUIT, 0, nullptr };
	KeDebugResponse res = _com->SendRequest(req);

	_dbg->IsConnected(false);

	if (res.header.status == DBG_STATUS_SUCCESS)
	{
		return DbgResponse::New(CMD_QUIT, res.header.status, "Debugger disconnected successfully", &res.header.context);
	}
	else
	{
		return DbgResponse::New(CMD_QUIT, res.header.status, "Quit command failed ! Debugger disconnected anyway.", context);
	}
}

DbgResponsePtr Command::CmdRegisters(vector<string> * args, KeDebugContext * context)
{
	KeDebugRequest req = { CMD_REGISTERS, 0, nullptr };
	KeDebugResponse res = _com->SendRequest(req);

	if (res.header.status == DBG_STATUS_SUCCESS)
	{
		RegistersX86 * regs = new RegistersX86(res.header.context);
		return DbgRegistersResponse::New(CMD_REGISTERS, res.header.status, regs->ToString(), regs, &res.header.context);
	}
	else
	{
		return DbgResponse::New(CMD_REGISTERS, res.header.status, "Registers command failed !", context);
	}
}

DbgResponsePtr Command::CmdDisass(vector<string> * args, KeDebugContext * context)
{
	const unsigned int DEFAULT_NB_INST = 10;
	unsigned int size = DEFAULT_NB_INST; // x inst to disassemble
	size_t nbArgs = args->size();

	if (nbArgs > 1)
	{
		throw DbgException("Too many arguments for Disass command");
	}
	else if (nbArgs == 1)
	{
		size = (unsigned int)std::stoi((*args)[0]);
	}

	return _CmdDisass(size, context);
}

DbgResponsePtr Command::_CmdDisass(unsigned int size, KeDebugContext * context)
{
	unsigned char * buffer = nullptr;
	KeDebugRequest request;
	KeDebugDisassParamReq param = { 0 };
	KeDebugDisassParamRes * paramRes = nullptr;

	param.nbInst = size;

	request.command = CMD_DISASS;
	request.paramSize = sizeof(KeDebugDisassParamReq);
	request.param = (char *)&param;

	KeDebugResponse response = _com->SendRequest(request);
	if (response.header.status != DBG_STATUS_SUCCESS)
	{
		throw DbgException("_CmdDisass() : SendRequest() failed with dbg status : " + response.header.status);
	}

	if (response.header.dataSize == 0 || response.data == nullptr)
	{
		throw DbgException("_CmdDisass() : unexpected empty data in response");
	}

	paramRes = (KeDebugDisassParamRes *)response.data;

	_disass.SetInputBuffer((unsigned char *)&paramRes->data, paramRes->size);
	string asmCode = _disass.Disassemble(paramRes->startingAddress, size);

	return DbgResponse::New(CMD_DISASS, response.header.status, asmCode, &(response.header.context));
}

DbgResponsePtr Command::CmdStackTrace(vector<string> * args, KeDebugContext * context)
{
	//unsigned char * buffer = nullptr;
	//unsigned int bufferSize = 0;

	//_com->SendByte(CMD_STACK_TRACE);
	//_com->ReadBytes((unsigned char *)&bufferSize, sizeof(unsigned int));

	//buffer = new unsigned char[bufferSize];

	//_com->ReadBytes((unsigned char*)buffer, bufferSize);

	//return SymbolsHelper::Instance()->Get((unsigned int *)buffer, bufferSize / sizeof(unsigned int));
	return DbgResponse::New(CMD_QUIT, DBG_STATUS_SUCCESS, "test", context);
}

DbgResponsePtr Command::CmdMemory(vector<string> * args, KeDebugContext * context)
{
	//const unsigned int DEFAULT_NB_BYTES = 50;
	//const unsigned int NB_BYTES_PER_LINE = 10;

	//unsigned int addr = 0;
	//unsigned int size = DEFAULT_NB_BYTES;
	//size_t nbArgs = args->size();
	//unsigned char * buffer = nullptr;
	//stringstream ss;
	//char memoryAccessible = 0;
	//
	//if (nbArgs < 1)
	//{
	//	return "Missing argument 'address'";
	//}

	//string stringAddr = (*args)[0];
	//if (stringAddr.size() > 2 && stringAddr[0] == '0' && stringAddr[1] == 'x')
	//{
	//	addr = std::stoul(stringAddr.substr(2, stringAddr.size()), nullptr, 16);
	//}
	//else
	//{
	//	addr = std::stoul(stringAddr, nullptr, 16);
	//}

	//if (nbArgs > 2)
	//{
	//	throw DbgException("Too many arguments for memory command");
	//}
	//else if (nbArgs == 2)
	//{
	//	size = std::stoi((*args)[1]);
	//}

	//buffer = new unsigned char[size];
	//if (buffer == nullptr)
	//{
	//	throw DbgException("CmdMemory() : Can't allocate memory for buffer");
	//}

	//_com->SendByte(CMD_MEMORY);
	//_com->SendBytes((unsigned char *)&addr, sizeof(unsigned int));
	//_com->SendBytes((unsigned char *)&size, sizeof(unsigned int));

	//memoryAccessible = _com->ReadByte();
	//if (memoryAccessible == 0)
	//{
	//	return "<memory not available>";
	//}

	//_com->ReadBytes(buffer, size);

	//ss << std::hex << addr << " ";
	//for (unsigned int i = 0; i < size; i++)
	//{
	//	if (i % NB_BYTES_PER_LINE == 0 && i != 0)
	//	{
	//		addr += NB_BYTES_PER_LINE;

	//		ss << endl;
	//		ss << std::hex << addr << " ";
	//	}

	//	if (buffer[i] <= 0xf)
	//		ss << std::hex << "0" << (unsigned int)buffer[i] << " ";
	//	else
	//		ss << std::hex << (unsigned int)buffer[i] << " ";
	//}

	//return ss.str();
	return DbgResponse::New(CMD_QUIT, DBG_STATUS_SUCCESS, "test", context);
}

DbgResponsePtr Command::CmdBreakpoint(std::vector<std::string> * args, KeDebugContext * context)
{
	//size_t nbArgs = 0;
	//unsigned int addr = 0;
	//string param;
	//stringstream ss;
	//static int breakpointId = 0;

	//nbArgs = args->size();

	//if (nbArgs != 1)
	//{
	//	throw DbgException("Invalid arguments number for breakpoint command");
	//}

	//param = (*args)[0];
	//if (param[0] != '0' || param[1] != 'x')
	//{
	//	SymbolsHelper::Instance()->GetAddrFromSymbol(param, addr);
	//}
	//else
	//{
	//	addr = std::stoul(param.substr(2, param.size()), nullptr, 16);
	//}

	//{
	//	_com->SendByte(CMD_BP);
	//	_com->SendBytes((unsigned char *)&addr, sizeof(unsigned int));

	//	char success = _com->ReadByte();
	//	if (success == 0)
	//	{
	//		ss << "Failed to set breakpoint on 0x" << std::hex << addr;
	//	}
	//	else
	//	{
	//		KeBreakpoint bp;
	//		bp.addr = addr;
	//		bp.id = BP_ENABLED;
	//		_dbg->AddBreakpoint(bp);
	//		ss << "Breakpoint " << breakpointId++ << " set on 0x" << std::hex << addr;
	//	}
	//}

	//return ss.str();
	return DbgResponse::New(CMD_QUIT, DBG_STATUS_SUCCESS, "test", context);
}

DbgResponsePtr Command::CmdBreakpointList(std::vector<std::string> * args, KeDebugContext * context)
{
	return DbgResponse::New(CMD_QUIT, DBG_STATUS_SUCCESS, "test", context);
}