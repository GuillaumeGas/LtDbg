#pragma once

#include <string>
#include <memory>

#include "Commands.hpp"
#include "LtKinc/ltkinc.h"

enum DbgStatus
{
	STATUS_SUCCESS,
	STATUS_FAILURE,
	STATUS_ALREADY_CONNECTED,
};

struct DbgResponse;
typedef std::shared_ptr<DbgResponse> DbgResponsePtr;

struct DbgResponse
{
	CommandId command;
	DbgStatus status;
	std::string content;
	KeDebugContext * context;

	DbgResponse(CommandId command, DbgStatus status, std::string content, KeDebugContext * context)
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
		return std::shared_ptr<DbgResponse>(new T(args...));
	}
};