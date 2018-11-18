#pragma once

#include <functional>
#include <map>

#include "Com.hpp"
#include "CmdList.hpp"

class Dbg;

class CommandManager
{
public:
	CommandManager(Dbg * dbg, Com * com);

	std::function<void()> & operator[](Command cmd);
	std::function<void()> & operator[](const std::string & str);
	bool CommandExists(const std::string & command) const;
	bool CommandExists(Command command) const;

	void CmdConnect();
	void CmdStep();
	void CmdContinue();
	void CmdQuit();
	void CmdRegisters();
	void CmdDisass();

private:
	Dbg * _dbg;
	Com * _com;
	std::map<Command, std::function<void()> > _commands;
};