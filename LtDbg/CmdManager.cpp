#include <iostream>

#include "CmdManager.hpp"
#include "Exceptions.hpp"
#include "Dbg.hpp"

// Includes all LtKernel header files
#include "LtKinc/ltkinc.h"

using namespace std;

CommandManager::CommandManager(Dbg * dbg, Com * com) : _dbg(dbg), _com(com) 
{
	_cmd = new Command(dbg, com);

	_commands[CMD_CONNECT] = std::bind(&Command::CmdConnect, _cmd);
	_commands[CMD_STEP] = std::bind(&Command::CmdStep, _cmd);
	_commands[CMD_CONTINUE] = std::bind(&Command::CmdContinue, _cmd);
	_commands[CMD_QUIT] = std::bind(&Command::CmdQuit, _cmd);
	_commands[CMD_REGISTERS] = std::bind(&Command::CmdRegisters, _cmd);
	_commands[CMD_DISASS] = std::bind(&Command::CmdDisass, _cmd);
	_commands[CMD_STACK_TRACE] = std::bind(&Command::CmdStackTrace, _cmd);
}

CommandManager::~CommandManager()
{
	if (_cmd != nullptr)
		delete _cmd;
}

std::function<std::string()> & CommandManager::operator[](CommandId cmd)
{
	return _commands[cmd];
}

std::function<std::string()> & CommandManager::operator[](const std::string & str)
{
	CommandId cmd = GetCmdFromStr(str);

	if (cmd == CMD_UNKNOWN)
		throw DbgException("Unknwo command !");

	if (!CommandExists(cmd))
		throw DbgException("Not implemented command !");

	return _commands[cmd];
}

bool CommandManager::CommandExists(const std::string & command) const
{
	CommandId cmd = GetCmdFromStr(command);

	if (cmd == CMD_UNKNOWN)
		return false;

	return _commands.find(cmd) != _commands.end();
}

bool CommandManager::CommandExists(CommandId command) const
{
	return _commands.find(command) != _commands.end();
}