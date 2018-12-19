#pragma once

#include <iostream>
#include <sstream>

#include "Commands.hpp"

class DbgException
{
public:
	DbgException() {}
	DbgException(const std::string msg) : _msg(msg) {}

	virtual std::string ToString() const
	{
		return _msg;
	}

protected:
	std::string _msg;
};

class UnknownCommandException : public DbgException
{
public:
	UnknownCommandException(const std::string command) : DbgException()
	{
		std::stringstream ss;
		ss << "Unknown command \"";
		ss << command;
		ss << "\"";
		_msg = ss.str();
	}

	UnknownCommandException(const CommandId commandId) : DbgException()
	{
		std::stringstream ss;
		ss << "Unknown command \"";
		ss << GetCmdValue(commandId);
		ss << "\" (" << commandId << ")";
		_msg = ss.str();
	}
};

class ComException : public DbgException
{
public:
	ComException(const std::string msg) : DbgException(msg) {}
};