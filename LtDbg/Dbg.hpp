#pragma once

#include "Com.hpp"
#include "CmdManager.hpp"

class Dbg
{
public:
	Dbg();
	~Dbg();

	void Start();
	bool GetConnectedState() const;
	void SetConnectedState(bool state);

private:
	void UserCommandLine(CommandManager & cm);

	Com * _com;
	CommandManager * _cm;
	bool _connected;
};