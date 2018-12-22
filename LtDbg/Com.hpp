#pragma once

#include <windows.h>
#include <string>

#include "LtKinc/ltkinc.h"

class Com
{
public:
	Com(std::string pipeName);
	~Com();

	void Connect();

	void SendPacket(const KeDebugPacket & packet);
	KeDebugPacket RecvPacket();

	KeDebugResponse SendRequest(const KeDebugRequest & request);
	KeDebugResponse RecvResponse();

	void SendByte(unsigned char byte);
	void SendBytes(unsigned char * buffer, unsigned int size);
	unsigned char ReadByte();
	void ReadBytes(unsigned char * buffer, unsigned int bufferSize);

private:
	KeDebugPacket MakePacket(const KeDebugRequest & request);
	void CleanupPacket(KeDebugPacket & packet);

	std::string _pipeName;
	HANDLE _pipeHandle;
};