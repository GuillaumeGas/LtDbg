#include "Com.hpp"

#include "Exceptions.hpp"

using namespace std;

constexpr DWORD PIPE_WAIT_TIMEOUT = 5000;

Com::Com(string pipeName) : _pipeName(pipeName), _pipeHandle(INVALID_HANDLE_VALUE) {}

Com::~Com()
{
	if (_pipeHandle != NULL || _pipeHandle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(_pipeHandle);
	}
}

void Com::Connect()
{
	while (true)
	{
		_pipeHandle = CreateFileA(_pipeName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

		if (_pipeHandle != INVALID_HANDLE_VALUE)
			break;

		const DWORD LAST_ERROR = GetLastError();

		if (LAST_ERROR != ERROR_PIPE_BUSY)
			throw ComException("Can't open pipe, CreateFileA() failed with code : " + std::to_string(LAST_ERROR));

		if (!WaitNamedPipeA(_pipeName.c_str(), PIPE_WAIT_TIMEOUT))
			throw ComException("Could not open pipe, WaitNamedPipeA() return FALSE, last error : " + std::to_string(LAST_ERROR));
	}
}

void Com::SendByte(unsigned char byte)
{
	BOOL success = FALSE;
	DWORD read = 0;

	success = WriteFile(_pipeHandle, &byte, sizeof(byte), &read, NULL);

	if (success == FALSE)
	{
		throw ComException("SendByte() : Com!WriteFile() failed with code " + std::to_string(GetLastError()));
	}
}

void Com::SendBytes(unsigned char * buffer, unsigned int size)
{
	BOOL success = FALSE;
	DWORD read = 0;

	success = WriteFile(_pipeHandle, buffer, size, &read, NULL);

	if (success == FALSE)
	{
		throw ComException("SendBytes() : Com!WriteFile() failed with code " + std::to_string(GetLastError()));
	}
}

unsigned char Com::ReadByte()
{
	BOOL success = FALSE;
	DWORD read = 0;
	unsigned char byte = 0;

	success = ReadFile(_pipeHandle, &byte, sizeof(unsigned char), &read, NULL);

	if (success == FALSE)
	{
		throw ComException("Com!ReadFile() failed with code " + std::to_string(GetLastError()));
	}

	return byte;
}

void Com::ReadBytes(unsigned char * buffer, unsigned int bufferSize)
{
	for (unsigned int i = 0; i < bufferSize; i++)
		buffer[i] = ReadByte();
}

KeDebugResponse Com::SendRequest(KeDebugRequest & req)
{
	unsigned int size = sizeof(KeDebugRequest) + req.paramSize;

	KeDebugPacket packet;
	packet.content = new u8[size];

	if (packet.content == nullptr)
	{
		throw ComException("SendRequest() failed : couldn't allocate memory for packet content");
	}

	packet.size = size;

	KeDebugRequest * innerReq = (KeDebugRequest *)packet.content;
	innerReq->command = req.command;
	innerReq->paramSize = req.paramSize;
	CopyMemory(&(innerReq->param), req.param, req.paramSize);

	SendBytes((unsigned char *)&packet.size, sizeof(unsigned int));
	SendBytes((unsigned char *)packet.content, packet.size);

	return RecvResponse();
}

KeDebugResponse Com::RecvResponse()
{
	KeDebugResponse res;

	ReadBytes((unsigned char *)&res.header, sizeof(KeDebugResponse));

	if (res.header.dataSize > 0)
	{
		res.data = new char[res.header.dataSize];
		if (res.data == nullptr)
		{
			throw ComException("RecvResponse() failed : couldn't allocate memory for response data");
		}

		ReadBytes((unsigned char *)res.data, res.header.dataSize);
	}
	else
	{
		res.data = nullptr;
	}

	return res;
}