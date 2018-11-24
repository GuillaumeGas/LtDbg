#pragma once

#include <windows.h>
#include <string>

class Com
{
public:
	Com(std::string pipeName);
	~Com();

	void Connect();
	void SendByte(unsigned char byte);
	unsigned char ReadByte();
	void ReadBytes(unsigned char * buffer, unsigned int bufferSize);

private:
	std::string _pipeName;
	HANDLE _pipeHandle;
};