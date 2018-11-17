#pragma once

#include <string>
#include <windows.h>

typedef unsigned char u8;

class Com
{
public:
	Com(std::string pipeName);
	~Com();

	void Connect();
	void SendByte(u8 byte);
	u8 ReadByte();

private:
	std::string _pipeName;
	HANDLE _pipeHandle;
};