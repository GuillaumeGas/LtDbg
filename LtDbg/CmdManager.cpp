#include <iostream>

#include "CmdManager.hpp"
#include "Exceptions.hpp"
#include "Dbg.hpp"

//#include "..\udis\BuildVS2010\Build\Include\udis86.h"
#include "..\udis\libudis86\extern.h"
//#include "..\udis\libudis86\

// Includes all LtKernel header files
#include "LtKinc/ltkinc.h"

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
		byte res = _com->ReadByte();
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
	_com->ReadBytes((byte*)&context, sizeof(DebugContext));

	cout << "eip : " << std::hex << context.eip << endl;
}

void CommandManager::CmdDisass()
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
}