#include <iostream>

#include "Dbg.hpp"

using namespace std;

const string PIPE_NAME = "\\\\.\\pipe\\ltdbgpipe";

Dbg::Dbg() : _connected(false)
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
	CommandManager cmdManager(this, _com);

	_com->Connect();

	cmdManager[CMD_CONNECT]();

	UserCommandLine(cmdManager);
}

void Dbg::UserCommandLine(CommandManager & cm)
{
	do
	{
		string command = "";

		cout << " > ";
		cin >> command;

		if (cm.CommandExists(command))
		{
			cout << cm[command]() << endl;
		}
	} while (_connected);
}

bool Dbg::GetConnectedState() const
{
	return _connected;
}

void Dbg::SetConnectedState(bool state)
{
	_connected = state;
}