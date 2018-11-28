#include <iostream>
#include <vector>

#include "Dbg.hpp"
#include "Utils/StringUtils.hpp"

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

void Dbg::Start(const char * kernelImagePath)
{
	CommandManager cmdManager(this, _com, kernelImagePath);

	_com->Connect();

	cmdManager[CMD_CONNECT](nullptr);

	UserCommandLine(cmdManager);
}

void Dbg::UserCommandLine(CommandManager & cm)
{
	cout << ">> LtKernel Debugger <<" << endl << endl;

	do
	{
		string input = "";

		cout << "kd> ";
		getline(cin, input);

		HandleInput(cm, input);
	} 
	while (_connected);
}

void Dbg::HandleInput(CommandManager & cm, string & input)
{
	vector<string> cmdLine = StringUtils::Split(input, ' ');
	string command = cmdLine[0];
	cmdLine.erase(cmdLine.begin());

	if (cm.CommandExists(command))
	{
		cout << cm[command](&cmdLine) << endl;
	}
	else
	{
		cout << "Unknown command !" << endl;
	}
}

bool Dbg::GetConnectedState() const
{
	return _connected;
}

void Dbg::SetConnectedState(bool state)
{
	_connected = state;
}