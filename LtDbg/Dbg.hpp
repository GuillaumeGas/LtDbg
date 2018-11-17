#pragma once

#include "Com.hpp"
#include "CmdManager.hpp"

class Dbg
{
public:
	Dbg();
	~Dbg();

	void Start();

private:
	Com * _com;
	CommandManager * _cm;
};