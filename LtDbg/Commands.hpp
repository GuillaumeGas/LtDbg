#pragma once

#include <vector>
#include <string>

#include "Utils/Disassembler.hpp"
#include "LtKinc/ltkinc.h"

//#define COMMANDS_LIST                     \
//	COMMAND(CMD_CONNECT,     "connect")   \
//	COMMAND(CMD_STEP,        "p")         \
//	COMMAND(CMD_CONTINUE,    "c")         \
//	COMMAND(CMD_QUIT,        "q")         \
//	COMMAND(CMD_REGISTERS,   "r")		  \
//	COMMAND(CMD_DISASS,      "d")		  \
//	COMMAND(CMD_STACK_TRACE, "s")		  \
//	COMMAND(CMD_MEMORY,      "m")		  \
//	COMMAND(CMD_BP,          "bp")        \
//	COMMAND(CMD_BL,          "bl")        \
//	COMMAND(CMD_UNKNOWN,     "<unknown>") \
//	COMMAND(CMD_END,         "<end>" )    \

//enum CommandId
//{
//#define COMMAND(name, _) name,
//#define COMMAND_KEYWORD(x, y) COMMAND(x, y)
//	COMMANDS_LIST
//#undef COMMAND
//#undef COMMAND_KEYWORD
//};

static CommandId GetCmdFromStr(const std::string & str) {
#define COMMAND(name, value)			\
	    if (str == value) return name;
#define COMMAND_KEYWORD(x, y) COMMAND(x, y)
	COMMANDS_LIST
#undef COMMAND_KEYWORD
#undef COMMAND
		return CMD_UNKNOWN;
}

static const char * GetCmdId(CommandId cmdId) {
	switch (cmdId) {
#define COMMAND(name, value)			\
		case name:			\
		    return #name;
#define COMMAND_KEYWORD(x, y) COMMAND(x, y)
		COMMANDS_LIST
#undef COMMAND_KEYWORD
#undef COMMAND
	default:
		return NULL;
	}
}

static const char * GetCmdValue(CommandId cmdId) {
	switch (cmdId) {
#define COMMAND(name, value)			\
		case name:			\
		    return value;
#define COMMAND_KEYWORD(x, y) COMMAND(x, y)
		COMMANDS_LIST
#undef COMMAND_KEYWORD
#undef COMMAND
	default:
		return NULL;
	}
}

class Dbg;
class Com;

class Command
{
public:
	Command(Dbg * dbg, Com * com, const std::string kenrelImagePath);

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

	void SetSymbolsPath(const std::string symbolsFileName);

private:
	Dbg * _dbg;
	Com * _com;
	Disassembler _disass;

	DbgResponsePtr _CmdDisass(unsigned int size, KeDebugContext * context);
};