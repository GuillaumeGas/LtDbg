#pragma once

#include <iostream>

class DbgException
{
public:
	DbgException(const std::string msg) : _msg(msg) {}

	virtual std::string ToString() const
	{
		return _msg;
	}

protected:
	std::string _msg;
};

class ComException : public DbgException
{
public:
	ComException(const std::string msg) : DbgException(msg) {}
};