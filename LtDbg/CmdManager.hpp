#pragma once

#include <functional>
#include <map>
#include <vector>

#include "Com.hpp"
#pragma once

#include "Commands.hpp"

class Dbg;
class Command;
struct KeDebugContext;

typedef DbgResponsePtr(CommandFunc)(std::vector<std::string>&);

class CommandManager
{
public:
    CommandManager(Dbg * dbg, Com * com);
	~CommandManager();

	std::function<DbgResponsePtr(std::vector<std::string>*, KeDebugContext *)> & operator[](CommandId cmd);
	std::function<DbgResponsePtr(std::vector<std::string>*, KeDebugContext *)> & operator[](const std::string & str);
	bool CommandExists(const std::string & command) const;
	bool CommandExists(CommandId command) const;

    void SetSymbolsPaths(const std::vector<std::string> symbolsFilePaths);

private:
	Dbg * _dbg;
	Com * _com;
	Command * _cmd;
	std::map<CommandId, std::function<DbgResponsePtr(std::vector<std::string>*, KeDebugContext*)> > _commands;
};
