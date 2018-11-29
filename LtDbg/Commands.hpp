#pragma once

#include <vector>
#include <string>

#include "Utils/Disassembler.hpp"

#define COMMANDS_LIST                     \
	COMMAND(CMD_CONNECT,     "connect")   \
	COMMAND(CMD_STEP,        "p")         \
	COMMAND(CMD_CONTINUE,    "c")         \
	COMMAND(CMD_QUIT,        "q")         \
	COMMAND(CMD_REGISTERS,   "r")		  \
	COMMAND(CMD_DISASS,      "d")		  \
	COMMAND(CMD_STACK_TRACE, "s")		  \
	COMMAND(CMD_MEMORY,      "m")		  \
	COMMAND(CMD_UNKNOWN,     "<unknown>") \
	COMMAND(CMD_END,         "<end>" )    \

enum CommandId
{
#define COMMAND(name, _) name,
#define COMMAND_KEYWORD(x, y) COMMAND(x, y)
	COMMANDS_LIST
#undef COMMAND
#undef COMMAND_KEYWORD
};

static CommandId GetCmdFromStr(const std::string & str) {
#define COMMAND(name, value)			\
	    if (str == value) return name;
#define COMMAND_KEYWORD(x, y) COMMAND(x, y)
	COMMANDS_LIST
#undef LEX_TOKEN_KEYWORD
#undef LEX_TOKEN
		return CMD_UNKNOWN;
}

class Dbg;
class Com;

class Command
{
public:
	Command(Dbg * dbg, Com * com, const char * kenrelImagePath);

	std::string CmdConnect(std::vector<std::string> * args);
	std::string CmdStep(std::vector<std::string> * args);
	std::string CmdContinue(std::vector<std::string> * args);
	std::string CmdQuit(std::vector<std::string> * args);
	std::string CmdRegisters(std::vector<std::string> * args);
	std::string CmdDisass(std::vector<std::string> * args);
	std::string CmdStackTrace(std::vector<std::string> * args);
	std::string CmdMemory(std::vector<std::string> * args);

private:
	Dbg * _dbg;
	Com * _com;
	Disassembler _disass;

	std::string _CmdDisass(unsigned int size);
};