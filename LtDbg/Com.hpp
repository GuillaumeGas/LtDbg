#pragma once

#include <string>
#include <windows.h>

class Com
{
public:
	Com(std::string pipeName);
	~Com();

	void Connect();
	void SendByte(byte byte);
	byte ReadByte();
	void ReadBytes(byte * buffer, unsigned int bufferSize);

private:
	std::string _pipeName;
	HANDLE _pipeHandle;
};