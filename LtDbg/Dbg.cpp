#include <iostream>
#include <vector>

#include "Dbg.hpp"
#include "Utils/StringUtils.hpp"
#include "Exceptions.hpp"

#include "LtKinc/ltkinc.h"

using namespace std;

Dbg::Dbg() : _connected(false), _com(nullptr), _cmdManager(nullptr)
{
}

Dbg::~Dbg()
{
	if (_cmdManager != nullptr)
		delete _cmdManager;

	if (_com != nullptr)
		delete _com;
}

void Dbg::Connect(const string pipeName)
{
	_com = new Com(pipeName);
	_com->Connect();
	_cmdManager = new CommandManager(this, _com, _symbolsPath);
}

DbgResponsePtr Dbg::ExecuteCommand(const string input, KeDebugContext * context)
{
	vector<string> paramsVec = StringUtils::Split(input, ' ');
	string command = paramsVec[0];
	paramsVec.erase(paramsVec.begin());

	if (_cmdManager->CommandExists(command))
	{
		return (*_cmdManager)[command](&paramsVec, context);
	}
	else
	{
		throw UnknownCommandException(command);
	}
}

DbgResponsePtr Dbg::ExecuteCommand(const CommandId commandId, const string params, KeDebugContext * context)
{
	vector<string> paramsVec = StringUtils::Split(params, ' ');
	if (_cmdManager->CommandExists(commandId))
	{
		return (*_cmdManager)[commandId](&paramsVec, context);
	}
	else
	{
		throw UnknownCommandException(commandId);
	}
}

DbgResponsePtr Dbg::WaitForBeakpointHit()
{
	KeDebugResponse res = _com->RecvResponse();
	return DbgResponse::New(res.header.command, res.header.status, "Breakpoint hit !", &(res.header.context));
}

void Dbg::SetSymbolsPath(const string symbolsFileName)
{
	_symbolsPath = symbolsFileName;
	_cmdManager->SetSymbolsPath(symbolsFileName);
}

bool Dbg::IsConnected() const
{
	return _connected;
}

void Dbg::IsConnected(bool state)
{
	_connected = state;
}

KeBreakpoint * Dbg::CheckIfBreakpointReached(u32 addr) const
{
	for (auto it : _breakpoints)
	{
		if (it.addr == addr)
		{
			return &it;
		}
	}

	return nullptr;
}

void Dbg::AddBreakpoint(KeBreakpoint bp)
{
	_breakpoints.push_back(bp);
}