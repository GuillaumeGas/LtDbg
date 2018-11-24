#pragma once

#include <functional>
#include <map>

#include "Com.hpp"
#pragma once

#include "CmdList.hpp"

class Dbg;
class Command;

class CommandManager
{
public:
	CommandManager(Dbg * dbg, Com * com, const char * kernelImagePath);
	~CommandManager();

	std::function<std::string()> & operator[](CommandId cmd);
	std::function<std::string()> & operator[](const std::string & str);
	bool CommandExists(const std::string & command) const;
	bool CommandExists(CommandId command) const;

private:
	Dbg * _dbg;
	Com * _com;
	Command * _cmd;
	std::map<CommandId, std::function<std::string()> > _commands;
};