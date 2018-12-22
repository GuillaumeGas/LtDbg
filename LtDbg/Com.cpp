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

void Com::SendPacket(const KeDebugPacket & packet)
{
	SendBytes((unsigned char *)&packet.size, sizeof(unsigned int));
	SendBytes((unsigned char *)packet.content, packet.size);
}

KeDebugPacket Com::RecvPacket()
{
	KeDebugPacket packet = { 0 };

	ReadBytes((unsigned char *)&packet.size, sizeof(unsigned int));

	if (packet.size == 0)
	{
		packet.content = nullptr;
		return packet;
	}

	packet.content = new u8[packet.size];
	if (packet.content == nullptr)
	{
		throw ComException("RecvPacket() : Couldn't allocate memory for packet.content");
	}

	ReadBytes((unsigned char *)packet.content, packet.size);

	return packet;
}

KeDebugPacket Com::MakePacket(const KeDebugRequest & request)
{
	KeDebugPacket packet = { 0 };

	packet.size = sizeof(KeDebugRequest) + request.paramSize;

	packet.content = new u8[packet.size];
	if (packet.content == nullptr)
	{
		throw ComException("MakePacket() : Couldn't allocate memory for packet.content");
	}

	std::memcpy(packet.content, &request, sizeof(KeDebugRequest));
	KeDebugRequest * tmpReq = (KeDebugRequest *)packet.content;
	std::memcpy(&(tmpReq->param), request.param, request.paramSize);

	return packet;
}

void Com::CleanupPacket(KeDebugPacket & packet)
{
	if (packet.content != nullptr)
	{
		delete[] packet.content;
		packet.content = nullptr;
	}
}

KeDebugResponse Com::SendRequest(const KeDebugRequest & request)
{
	KeDebugPacket packet = MakePacket(request);
	SendPacket(packet);

	return RecvResponse();
}

KeDebugResponse Com::RecvResponse()
{
	KeDebugPacket packet = RecvPacket();
	KeDebugResponse * dataResponse = (KeDebugResponse *)packet.content;
	KeDebugResponse response;

	response.header = dataResponse->header;

	if (response.header.dataSize > 0)
	{
		response.data = new char[response.header.dataSize];
		if (response.data == nullptr)
		{
			throw ComException("RecvResponse() : Couldn't allocate memory for response.data");
		}

		std::memcpy(response.data, &(dataResponse->data), response.header.dataSize);
	}

	CleanupPacket(packet);
	dataResponse = nullptr;

	return response;
}