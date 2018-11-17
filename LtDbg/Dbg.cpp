#include <iostream>

#include "Dbg.hpp"

using namespace std;

const string PIPE_NAME = "\\\\.\\pipe\\ltdbgpipe";

Dbg::Dbg()
{
	_com = new Com(PIPE_NAME);
}

Dbg::~Dbg()
{
	if (_com == nullptr)
		delete _com;
}

void Dbg::Start()
{
	CommandManager cmdManager(_com);

	_com->Connect();

	cmdManager[CMD_CONNECT]();
}