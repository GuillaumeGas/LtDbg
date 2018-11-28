#pragma once

#include "Com.hpp"
#include "CmdManager.hpp"
#include "Commands.hpp"

class Dbg
{
public:
	Dbg();
	~Dbg();

	void Start(const char * kernelImagePath);
	bool GetConnectedState() const;
	void SetConnectedState(bool state);

private:
	void UserCommandLine(CommandManager & cm);
	void HandleInput(CommandManager & cm, std::string & input);

	Com * _com;
	bool _connected;
};