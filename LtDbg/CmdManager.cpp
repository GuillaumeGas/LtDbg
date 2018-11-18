#include <iostream>

#include "CmdManager.hpp"
#include "Exceptions.hpp"
#include "Dbg.hpp"

// Includes all LtKernel header files
#include "LtKinc/ltkinc.h"

typedef unsigned char u8;

using namespace std;

CommandManager::CommandManager(Dbg * dbg, Com * com) : _dbg(dbg), _com(com) 
{
	_commands[CMD_CONNECT] = std::bind(&CommandManager::CmdConnect, this);
	_commands[CMD_STEP] = std::bind(&CommandManager::CmdStep, this);
	_commands[CMD_CONTINUE] = std::bind(&CommandManager::CmdContinue, this);
	_commands[CMD_QUIT] = std::bind(&CommandManager::CmdQuit, this);
	_commands[CMD_REGISTERS] = std::bind(&CommandManager::CmdRegisters, this);
	_commands[CMD_DISASS] = std::bind(&CommandManager::CmdDisass, this);
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
		u8 res = _com->ReadByte();
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
	_com->ReadBytes((u8*)&context, sizeof(DebugContext));

	cout << "eip : " << std::hex << context.eip << endl;
}

void CommandManager::CmdDisass()
{

}