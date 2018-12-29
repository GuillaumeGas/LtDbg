#pragma once

#include <vector>

#include "Com.hpp"
#include "CmdManager.hpp"
#include "Commands.hpp"
#include "Response.hpp"
#include "LtKinc/ltkinc.h"

class Dbg
{
public:
	Dbg();
	~Dbg();

	void Connect(const std::string pipeName);
	DbgResponsePtr ExecuteCommand(const std::string input, KeDebugContext * context = nullptr);
	DbgResponsePtr ExecuteCommand(const CommandId command, const std::string params = "", KeDebugContext * context = nullptr);
	DbgResponsePtr WaitForBeakpointHit();
	void SetSymbolsPath(const std::string symbolsFileName);

	bool IsConnected() const;
	void IsConnected(bool state);
	void AddBreakpoint(KeBreakpoint bp);

private:
	KeBreakpoint * CheckIfBreakpointReached(u32 addr) const;

	Com * _com;
	CommandManager * _cmdManager;
	bool _connected;
	std::vector<KeBreakpoint> _breakpoints;
	std::string _symbolsPath;
};