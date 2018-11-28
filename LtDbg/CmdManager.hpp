#pragma once

#include <functional>
#include <map>
#include <vector>

#include "Com.hpp"
#pragma once

#include "Commands.hpp"

class Dbg;
class Command;

typedef std::string(CommandFunc)(std::vector<std::string>&);

class CommandManager
{
public:
	CommandManager(Dbg * dbg, Com * com, const char * kernelImagePath);
	~CommandManager();

	std::function<std::string(std::vector<std::string>*)> & operator[](CommandId cmd);
	std::function<std::string(std::vector<std::string>*)> & operator[](const std::string & str);
	bool CommandExists(const std::string & command) const;
	bool CommandExists(CommandId command) const;

private:
	Dbg * _dbg;
	Com * _com;
	Command * _cmd;
	std::map<CommandId, std::function<std::string(std::vector<std::string>*)> > _commands;
};