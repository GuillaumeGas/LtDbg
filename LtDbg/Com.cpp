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

void Com::SendByte(u8 byte)
{
	BOOL success = FALSE;
	DWORD read = 0;

	success = WriteFile(_pipeHandle, &byte, sizeof(u8), &read, NULL);

	if (success == FALSE)
	{
		throw ComException("Com!WriteFile() failed with code " + std::to_string(GetLastError()));
	}
}

u8 Com::ReadByte()
{
	BOOL success = FALSE;
	DWORD read = 0;
	u8 byte = 0;

	success = ReadFile(_pipeHandle, &byte, sizeof(u8), &read, NULL);

	if (success == FALSE)
	{
		throw ComException("Com!ReadFile() failed with code " + std::to_string(GetLastError()));
	}

	return byte;
}

void Com::ReadBytes(u8 * buffer, unsigned int bufferSize)
{
	for (unsigned int i = 0; i < bufferSize; i++)
		buffer[i] = ReadByte();
}