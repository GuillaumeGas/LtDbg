#pragma once

#include <functional>
#include <map>

#include "Com.hpp"
#include "CmdList.hpp"

class CommandManager
{
public:
	CommandManager(Com * com);

	std::function<void()> & operator[](Command cmd);

	void CmdConnect();

private:
	Com * _com;
	std::map<Command, std::function<void()> > _commands;
};