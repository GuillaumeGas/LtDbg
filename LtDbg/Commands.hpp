#pragma once

#include <vector>
#include <string>

#include "Utils/Disassembler.hpp"
#include "LtKinc/ltkinc.h"
#include "Response.hpp"

class Dbg;
class Com;

struct DbgResponse;
typedef std::shared_ptr<DbgResponse> DbgResponsePtr;

class Command
{
public:
    Command(Dbg * dbg, Com * com);

	DbgResponsePtr CmdConnect(std::vector<std::string> * args, KeDebugContext * context);
	DbgResponsePtr CmdStep(std::vector<std::string> * args, KeDebugContext * context);
	DbgResponsePtr CmdContinue(std::vector<std::string> * args, KeDebugContext * context);
	DbgResponsePtr CmdQuit(std::vector<std::string> * args, KeDebugContext * context);
	DbgResponsePtr CmdRegisters(std::vector<std::string> * args, KeDebugContext * context);
	DbgResponsePtr CmdDisass(std::vector<std::string> * args, KeDebugContext * context);
	DbgResponsePtr CmdStackTrace(std::vector<std::string> * args, KeDebugContext * context);
	DbgResponsePtr CmdMemory(std::vector<std::string> * args, KeDebugContext * context);
	DbgResponsePtr CmdBreakpoint(std::vector<std::string> * args, KeDebugContext * context);
	DbgResponsePtr CmdBreakpointList(std::vector<std::string> * args, KeDebugContext * context);
    DbgResponsePtr CmdIdt(std::vector<std::string> * args, KeDebugContext * context);

private:
	Dbg * _dbg;
	Com * _com;
	Disassembler _disass;

	DbgResponsePtr _CmdDisass(unsigned int size, KeDebugContext * context);
};
