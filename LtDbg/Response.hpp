#pragma once

#include <string>
#include <memory>

#include "Utils/Registers.hpp"
#include "LtKinc/ltkinc.h"

struct DbgResponse;
typedef std::shared_ptr<DbgResponse> DbgResponsePtr;

struct DbgResponse
{
	CommandId command;
    KeDebugStatus status;
	std::string content;
	KeDebugContext * context;

	DbgResponse(CommandId command, KeDebugStatus status, std::string content, KeDebugContext * context)
	{
		this->command = command;
		this->status = status;
		this->content = content;
		this->context = nullptr;

		if (context != nullptr)
		{
			this->context = new KeDebugContext;
			*(this->context) = *context;
		}
	}

	~DbgResponse()
	{
		if (context != nullptr)
			delete context;
	}

	template<class... Args>
	static DbgResponsePtr New(Args&&... args) {
		return std::shared_ptr<DbgResponse>(new DbgResponse(args...));
	}
};

struct DbgRegistersResponse : public DbgResponse
{
	RegistersX86 * regs;

	DbgRegistersResponse(CommandId command, KeDebugStatus status, std::string content, RegistersX86 * registers, KeDebugContext * context) :
		DbgResponse(command, status, content, context)
	{
		this->regs = registers;
	}

	~DbgRegistersResponse()
	{
		if (regs != nullptr)
			delete regs;
	}

	template<class... Args>
	static DbgResponsePtr New(Args&&... args) {
		return std::shared_ptr<DbgRegistersResponse>(new DbgRegistersResponse(args...));
	}
};
