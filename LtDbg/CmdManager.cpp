#include <iostream>

#include "CmdManager.hpp"
#include "Exceptions.hpp"

typedef unsigned char u8;

CommandManager::CommandManager(Com * com) : _com(com) 
{
	_commands[CMD_CONNECT] = std::bind(&CommandManager::CmdConnect, this);
}

std::function<void()> & CommandManager::operator[](Command cmd)
{
	return _commands[cmd];
}

void CommandManager::CmdConnect()
{
	while (1)
	{
		_com->SendByte(1);
		u8 res = _com->ReadByte();
		if (res == 1)
			break;
	}
	
	std::cout << "Connected to LtKernel !" << std::endl;
}